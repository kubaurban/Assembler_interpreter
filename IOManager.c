#include "stdtools.h"
#include "memory.h"
#include "interpreter.h"


FILE* getInput(FILE* inputf, char* source, int argc, char* argv[])
{
	char* temp;

	if (argc != 2)						// nie podano sciezki wzglednej do pliku z instrukcjami pseudoasemblera
	{
		temp = malloc((BYTE_LENGTH + 1) * sizeof(char));
		printf("Podaj sciezke wzgledna do pliku tekstowego z instrukcjami dla interpretera pseudoasemblera:\n");
		scanf("%s", temp);

		if (temp != NULL)
		{
			inputf = fopen(temp, "r");
			strncpy(source, temp, strlen(temp) - 4); // wycina nazwe pliku zrodlowego bez .txt
			free(temp);
		}
		else
		{
			inputf = NULL;
		}
	}
	else
	{
		inputf = fopen(argv[1], "r");	// wskazanie na plik tekstowy z instrukcjami pseudoasemblera podany jako argument przy wywolaniu programu
	}

	while (inputf == NULL) {
		temp = handleFileOpenError();
		inputf = fopen(temp, "r");
		strncpy(source, temp, strlen(temp) - 4); // wycina nazwe pliku zrodlowego bez .txt
		free(temp);
	}

	return inputf;
}
void readCode(FILE* inputf)
{
	char line[MAX_LINE_LENGTH + 1];
	char label[MAX_LABEL_LENGTH + 1];
	char sign[MAX_SIGN_LENGTH + 1];
	char arguments[MAX_LINE_LENGTH - 15 + 1];	// 15 znakow jest zarezerwowanych na etykiete i kod rozkazu, od 16 kolumny podawane sa argumenty
	char* token1, * token2, * token3;

	while (!readLineAndCheck(line, inputf))		// pobiera dyrektywy i je realizuje
	{
		token1 = strtok(line, " \r\n\t");
		token2 = strtok(NULL, " \r\n\t");
		token3 = strtok(NULL, " \r\n\t");

		if (token3 == NULL) {						// dyrektywa bez etykiety
			strcpy(sign, token1);
			strcpy(arguments, token2);
			interpretDiv("", sign, arguments);
		}
		else										// dyrektywa z etykieta
		{
			strcpy(label, token1);
			strcpy(sign, token2);
			strcpy(arguments, token3);
			if (isdigit(*label)) exit(1);			// jest to etykieta niezgodna z zalozeniami (zaczyna sie od cyfry)

			interpretDiv(label, sign, arguments);
		}

		memset(line, 0, (MAX_LINE_LENGTH + 1) * sizeof(char));
	}

	while (!readLineAndCheck(line, inputf))		// pobiera rozkazy i je realizuje
	{
		sscanf(line, "%10[ a-zA-Z0-9_] %[A-Z] %255[ a-zA-Z0-9_,()]\n", label, sign, arguments);
		if (isdigit(*label)) exit(1);			// jest to etykieta niezgodna z zalozeniami (zaczyna sie od cyfry)
		interpretOrd(label, sign, arguments);
		memset(line, 0, (MAX_LINE_LENGTH + 1) * sizeof(char));
	}

	fclose(inputf);
}
char* handleFileOpenError()
{
	char* arg;

	arg = malloc((MAX_LINE_LENGTH + 1) * sizeof(char));
	if (arg == NULL) exit(1);

	printf("Blad odczytu!\nPodaj sciezke wzgledna do pliku tekstowego z instrukcjami dla interpretera pseudoasemblera:\n");
	scanf("%s", arg);
	return arg;
}
int readLineAndCheck(char* buff, FILE* inputf)
{
	char* result;

	result = fgets(buff, (MAX_LINE_LENGTH + 1) * sizeof(char), inputf); // wczytanie linii tekstu z pliku wejsciowego

	if (result == NULL)
	{
		if (!feof(inputf)) exit(1);										// blad w przypadku gdy inputf == NULL, ale nie doszedl do konca pliku
		return 1;														// inputf == NULL poniewaz zakonczyl poprawnie odczyt z calego pliku
	}
	if (strcmp(result, "\n") == 0)										// zostala odczytana pusta linia (\n)
		return 1;
	if (strcmp(result, "\r\n") == 0)									// zostala odczytana pusta linia (\r\n)
		return 1;
	return 0;
}

void saveDataSection(FILE* outputf)
{
	char* word;
	char byte[3];
	unsigned int i, j;

	for (i = 0; i < strlen((char*)getFromRegistry(15).pval) - 1; i += 8)
	{
		word = getStringFromSection((char*)getFromRegistry(15).pval + i + 0, 8);

		for (j = 0; j < 6; j += 2)
		{
			sscanf((word + j), "%02s", byte);
			fprintf(outputf, "%s ", byte);	// wypisanie pojedynczego bajtu w postaci dwoch cyfr szesnastkowych
			memset(byte, 0, 2);
		}
		sscanf((word + j), "%02s", byte);
		fprintf(outputf, "%s", byte);		// zapis ostatniego bajtu kazdej danej jest juz bez oddzielajacej spacji
		memset(byte, 0, 2);

		fprintf(outputf, "\n");
		free(word);
	}
}
void saveDirectiveSection(FILE* outputf)
{
	char* word;
	char byte[3];
	unsigned int i, j;

	for (i = 0; i < strlen((char*)getFromRegistry(14).pval) - 1; i += 4)
	{
		fprintf(outputf, "\n");

		if (!isdigit(*((char*)getFromRegistry(14).pval + i))) // sprawdzenie czy pierwsza cyfra szesnastkowa jest cyfra dziesietna, jezeli tak to znaczy ze nastepny rozkaz do odczytania ma 2B, w przeciwnym razie ma on 4B
		{					// 4 BAJTOWY ROZKAZ

			word = getStringFromSection((char*)getFromRegistry(14).pval + i + 0, 8);

			for (j = 0; j < 6; j += 2)
			{
				sscanf((word + j), "%02s", byte);
				fprintf(outputf, "%s ", byte);				// wypisanie pojedynczego bajtu w postaci dwoch cyfr szesnastkowych
				memset(byte, 0, 2);
			}
			sscanf((word + j), "%02s", byte);
			fprintf(outputf, "%s", byte);					// zapis ostatniego bajtu rozkazu jest juz bez oddzielajacej spacji
			memset(byte, 0, 2);
			i += 4;
		}
		else				// 2 BAJTOWY ROZKAZ
		{
			word = getStringFromSection((char*)getFromRegistry(14).pval + i + 0, 4);

			j = 0;
			sscanf((word + j), "%02s", byte);
			fprintf(outputf, "%s ", byte);
			memset(byte, 0, 2);
			j += 2;
			sscanf((word + j), "%02s", byte);
			fprintf(outputf, "%s", byte);					// zapis drugiego bajtu rozkazu jest juz bez oddzielajacej spacji
			memset(byte, 0, 2);
		}
		free(word);
	}
}
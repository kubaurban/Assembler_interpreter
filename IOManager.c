#include "stdtools.h"
#include "memory.h"
#include "interpreter.h"


FILE* getInput(FILE* fp, char* source, int argc, char* argv[])
{
	char temp[MAX_LINE_LENGTH + 1];
	char* token1, * token2, * line, * t;

	if (argc < 2)						// nie podano sciezki wzglednej do pliku z instrukcjami pseudoasemblera (brak argumentów)
	{
		token1 = NULL;
		while (token1 == NULL) {
			printf("Podaj sciezke wzgledna do pliku tekstowego oraz typ tego pliku:\n");
			fgets(temp, (MAX_LINE_LENGTH + 1) * sizeof(char), stdin);
			token1 = strtok(temp, " \r\n\t");
		}

		fp = fopen(token1, "r");
		strncpy(source, token1, strlen(token1) - 4);	// wycina nazwe pliku zrodlowego bez .txt
		source[strlen(token1) - 4] = '\0';
		token1 = strtok(NULL, " \r\n\t");			// wycina drugi argument (o ile zostal podany, jesli nie, pozostaje NULLem)
	}
	else
	{
		fp = fopen(argv[1], "r");	// wskazanie na plik tekstowy z instrukcjami pseudoasemblera podany jako argument przy wywolaniu programu
		strncpy(source, argv[1], strlen(argv[1]) - 4); // wycina nazwe pliku zrodlowego bez .txt
		source[strlen(argv[1]) - 4] = '\0';
		token1 = NULL;
		if (argc == 3)
			token1 = argv[2];
	}

	while (fp == NULL) {
		line = handleFileOpenError();
		token2 = strtok(line, " \r\n\t");
		fp = fopen(token2, "r");
		strncpy(source, token2, strlen(token2) - 4); // wycina nazwe pliku zrodlowego bez .txt
		source[strlen(token2) - 4] = '\0';
		free(line);
	}


	if (token1 == NULL || strcmp(token1, "psa_code") == 0)
	{
		readPAInstructions(fp);
		fclose(fp);

		t = calloc(strlen(source) + 8, sizeof(char));
		if (t == NULL) exit(10);
		strcpy(t, source);

		strcat(t, "_machine.txt");
		fp = fopen(t, "w");
		saveDataSection(fp);
		saveDirectiveSection(fp);
	}
	else if(strcmp(token1, "msck_code") == 0)
	{
		readMachineCode(fp);
	}
	else
	{
		printf("B³êdny format");
		exit(1);
	}
	fclose(fp);

	return fp;
}
void readPAInstructions(FILE* inputf)
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
			if (isdigit(*label)) exit(9);			// jest to etykieta niezgodna z zalozeniami (zaczyna sie od cyfry)

			interpretDiv(label, sign, arguments);
		}

		memset(line, 0, (MAX_LINE_LENGTH + 1) * sizeof(char));
	}

	while (!readLineAndCheck(line, inputf))		// pobiera rozkazy i je realizuje
	{
		sscanf(line, "%10[ a-zA-Z0-9_] %[A-Z] %255[ a-zA-Z0-9_,()]\n", label, sign, arguments);
		if (isdigit(*label)) exit(9);				// jest to etykieta niezgodna z zalozeniami (zaczyna sie od cyfry)
		interpretOrd(label, sign, arguments);
		memset(line, 0, (MAX_LINE_LENGTH + 1) * sizeof(char));
	}
}
void readMachineCode(FILE* inputf)
{
	char line[MAX_LINE_LENGTH + 1];
	char* token;

	while (!readLineAndCheck(line, inputf))		// pobiera kod maszynowy i zapisuje go w sekcji danych
	{
		token = strtok(line, " \r\n\t");

		while (token)
		{
			storeInDataSection(token);
			token = strtok(NULL, " \r\n\t");
		}
		memset(line, 0, (MAX_LINE_LENGTH + 1) * sizeof(char));
	}

	while (!readLineAndCheck(line, inputf))		// pobiera kod maszynowy i zapisuje go w sekcji rozkazow
	{
		token = strtok(line, " \r\n\t");

		while (token)
		{
			storeInDirectiveSection(token);
			token = strtok(NULL, " \r\n\t");
		}
		memset(line, 0, (MAX_LINE_LENGTH + 1) * sizeof(char));
	}
}
char* handleFileOpenError()
{
	char* arg;

	arg = malloc((MAX_LINE_LENGTH + 1) * sizeof(char));
	if (arg == NULL) exit(10);

	printf("Blad odczytu!\nPodaj sciezke wzgledna do pliku tekstowego:\n");
	fgets(arg, (MAX_LINE_LENGTH + 1) * sizeof(char), stdin);
	return arg;
}
int readLineAndCheck(char* buff, FILE* inputf)
{
	char* result;

	result = fgets(buff, (MAX_LINE_LENGTH + 1) * sizeof(char), inputf); // wczytanie linii tekstu z pliku wejsciowego

	if (result == NULL)
	{
		if (!feof(inputf)) exit(8);										// blad w przypadku gdy inputf == NULL, ale nie doszedl do konca pliku
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

	for (i = 0; i < strlen((char*)getFromRegistry(15)) - 1; i += 8)
	{
		word = getStringFromSection((char*)getFromRegistry(15) + i + 0, 8);

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
	char* word = NULL;
	char byte[3];
	unsigned int i, j;

	for (i = 0; i < strlen((char*)getFromRegistry(14)) - 1; i += 4)
	{
		fprintf(outputf, "\n");

		if (!isdigit(*((char*)getFromRegistry(14) + i))) // sprawdzenie czy pierwsza cyfra szesnastkowa jest cyfra dziesietna, jezeli tak to znaczy ze nastepny rozkaz do odczytania ma 2B, w przeciwnym razie ma on 4B
		{					// 4 BAJTOWY ROZKAZ

			word = getStringFromSection((char*)getFromRegistry(14) + i + 0, 8);

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
			word = getStringFromSection((char*)getFromRegistry(14) + i + 0, 4);

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
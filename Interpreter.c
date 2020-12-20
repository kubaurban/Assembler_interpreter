#include "stdtools.h"
#include "interpreter.h"
#include "memory.h"
#include "labelcom.h"
/**
*Dlugosc slowa INTEGER.
*/
#define N 7


/**
Tablica List to tabela par znak_rozkazu - kod_rozkazu uzupelniona dla kazdego rozkazu.
*/
struct ordCode List[] = {
	"AR",	"10",
	"A",	"D1",
	"SR",	"12",
	"S",	"D3",
	"MR",	"14",
	"M",	"D5",
	"DR",	"16",
	"D",	"D7",
	"CR",	"18",
	"C",	"D9",
	"J",	"E0",
	"JZ",	"E1",
	"JP",	"E2",
	"JN",	"E3",
	"L",	"F0",
	"LR",	"31",
	"LA",	"F2",
	"ST",	"F3",
};

/**
* Zamienia ciag znakow zawierajacy liczbe decymalna na ciag znakow reprezentujacy te liczbe heksadecymalnie.
Argument hexBitAmount to liczba znakow szesnastkowych jaka spodziewamy sie otrzymac na wyjsciu.
*/
char* intoHex(char*, int);
/**
* Usuwa spacje ze stringa i zwraca na niego wskaznik.
*/
char* deleteSpaces(char*);
/**
* Nadaje rozkazowi kod rozkazu zapisujac go do bufora podanego w argumencie. Przyporz¹dkowanie rozkazu do jego kodu
znajduje sie w strukturze ordCode, a zbior wszystkich tych przyporzadkowan znajduje sie w tablicy List.
*/
void giveOrdCode(char*, char*);

void interpretDiv(char divLabel[], char* divSign, char* divArgs)
{
	unsigned int i, k;
	/*
	* buffer2 to bufor roboczy uzywany tam, gdzie wczytywany do niego string nie wymaga wiecej niz MAX_LABEL_LENGTH (10) znakow.
	* Wykorzystuje go przede wszystkim, aby wyciac odpowiedni krotszy napis z dluzszego napisu.
	*/
	char buffer2[MAX_LABEL_LENGTH + 1];
	char* hex, * temp1;
	/*
	* Wypelniona ~ tablica emptyWord reprezentuje zarezerwowana przez dyrektywe DS pamiec 4 bajtow.
	*/
	char emptyWord8[] = { '~', '~',  '~',  '~',  '~',  '~',  '~',  '~', '\0' };

	if (strcmp(divLabel, "") != 0)		//jesli dyrektywa posiada etykiete to zapisuje ja wraz z adresem jaki reprezentuje
		saveLabelAsAddress(divLabel, 15, (unsigned short)strlen((char*)getFromRegistry(15)) / 2, "");

	temp1 = strchr(divArgs, 42);		//zwraca wskaznik na pozycje gdzie znajduje sie znak '*' (kod ASCII = 42) lub wartosc NULL jesli nie wystepuje

	memset(buffer2, 0, MAX_LABEL_LENGTH + 1);

	if (temp1 == NULL)	// na podstawie informacji, czy w argumencie dyrektywy znajduje sie znak '*' mozemy obsluge dyrektyw podzielic na dwa bloki:
	{
		// 1 BLOK

		if (strcmp(divSign, "DC") == 0)		// OBSLUGA DYREKTYW POSTACI: <etykieta> DC INTEGER(<liczba_calkowita>)
		{
			if (strchr(divArgs, 41) == NULL) exit(1);					// jesli nie domknieto nawiasu przy podawaniu argumentu przerwij program z kodem 1

			strncat(buffer2, divArgs + N + 1, strlen(divArgs) - N - 2); // wycina fragment <liczba_calkowita> do buffer2

			hex = intoHex(buffer2, 8);
			storeInDataSection(hex);
			free(hex);
		}

		else								// OBSLUGA DYREKTYW POSTACI: <etykieta> DS INTEGER
		{
			storeInDataSection(emptyWord8);
		}
	}


	else				// 2 BLOK
	{
		strncat(buffer2, divArgs, strlen(divArgs) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> do buffer2
		k = atoi(buffer2);
		memset(buffer2, 0, (MAX_LABEL_LENGTH + 1) * sizeof(char));


		if (strcmp(divSign, "DC") == 0)		// OBSLUGA DYREKTYW POSTACI: <etykieta> DC <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>)
		{
			if (strchr(divArgs, 41) == NULL) exit(1);				// jesli nie domknieto nawiasu przy podawaniu argumentu przerwij program z kodem 1

			strncat(buffer2, temp1 + N + 2, strlen(temp1) - N - 3);	// wycina fragment <liczba_calkowita> do buffer2

			for (i = 0; i < k; i++)
			{
				hex = intoHex(buffer2, 8);
				storeInDataSection(hex);
				free(hex);
			}
		}

		else								// OBSLUGA DYREKTYW POSTACI: <etykieta> DS <liczba_komorek_pamieci>*INTEGER
		{
			for (i = 0; i < k; i++)
			{
				storeInDataSection(emptyWord8);
			}
		}
	}
}
void interpretOrd(char ordLabel[], char* ordSign, char* ordArgs)
{
	unsigned int i;
	/**
	* buffer to najwazniejszy bufor w programie, przechowuje (najpierw) kolejne skladowe kodu maszynowego rozkazu
	* (aby koncowo przechowywac caly kod maszynowy rozkazu) przed zapisaniem go w odpowiedniej sekcji oraz wczytaniem
	* do pliku wynikowego.
	*/
	char buffer[MAX_CODE_LENGTH + 1];
	/**
	* buffer2 to bufor roboczy uzywany tam, gdzie wczytywany do niego string nie wymaga wiecej niz MAX_LABEL_LENGTH (10) znakow.
	* Wykorzystuje go przede wszystkim, aby wyciac odpowiedni krotszy napis z dluzszego napisu.
	*/
	char buffer2[MAX_LABEL_LENGTH + 1];
	char* hex, * temp1, * temp2;
	struct labelledCommand* ptr;

	ordLabel = deleteSpaces(ordLabel);
	ordArgs = deleteSpaces(ordArgs);

	memset(buffer, 0, MAX_CODE_LENGTH + 1);
	giveOrdCode(buffer, ordSign);

	temp1 = strchr(ordArgs, 44);		//zwraca wskaznik na pozycje gdzie znajduje sie znak ',' (kod ASCII = 44) lub wartosc NULL jesli nie wystepuje					
	temp2 = ordArgs;

	if (temp1 != NULL)		// na podstawie informacji, czy w argumentach rozkazu znajduje sie znak ',' mozemy obsluge rozkazow podzielic na dwa bloki:
	{
		// 1 BLOK: OBSLUGA ROZKAZOW ARYTMETYCZNYCH I ZAPISYWANIA WARTOSCI

		if (strcmp(ordLabel, "") != 0)						// jesli rozkaz posiada etykiete to zapisuje ja wraz z adresem jaki reprezentuje
			saveLabelAsAddress(ordLabel, 14, (unsigned short)strlen((char*)getFromRegistry(14)) / 2, "");

		for (i = 0; temp2 != temp1; i++)					// wyodrêbnienie numeru pierwszego rejestru
		{
			buffer2[i] = *temp2;
			temp2++;
		}
		hex = intoHex(buffer2, 1);
		strcat(buffer, hex);								// zapis wyodrebnionego rejestru nr 1 do buffer
		free(hex);
		memset(buffer2, 0, (MAX_LABEL_LENGTH + 1) * sizeof(char));

		temp2++;
		for (i = 0; *temp2 != '\0' && *temp2 != '('; i++)	// wyodrêbnienie do buffer2: drugiego REJESTRU ( format rejestr,REJESTR ),
		{													// ETYKIETY ( format rejestr,ETYKIETA ) lub PRZESUNIECIA ( format rejestr,PRZESUNIECIE(rejestr) )
			buffer2[i] = *temp2;
			temp2++;
		}


		if (strlen(ordSign) == 2	// OBSLUGA ROZKAZOW TYPU REJESTR - REJESTR
			&& *(ordSign + 1) == 'R')
		{
			hex = intoHex(buffer2, 1);
			strcat(buffer, hex);							// zapis wyodrebnionego rejestru nr 2 do buffer
			free(hex);
		}

		else						// OBSLUGA ROZKAZOW TYPU REJESTR - PAMIEC
		{
			if (isdigit(*(temp1 + 1)))		// a). pamiec podawana w rozkazie jest w formacie PRZESUNIECIE(NR_REJESTRU) - etykieta nie moze zaczynac sie liczba
			{
				if (strchr(ordArgs, 41) == NULL) exit(1);	// jesli nie domknieto nawiasu przy podawaniu argumentu przerwij program z kodem 1

				if (strchr(ordArgs, 40) == NULL) exit(1);	// jesli nie otwarto nawiasu przy podawaniu argumentu przerwij program z kodem 1

				temp1 = calloc((MAX_BIAS_LENGTH + 1), sizeof(char));
				if (temp1 == NULL) exit(1);

				strcpy(temp1, buffer2);						// temp1 wskazuje na wyodrebnione z rozkazu PRZESUNIECIE
				memset(buffer2, 0, (MAX_LABEL_LENGTH + 1) * sizeof(char));

				temp2++;
				for (i = 0; isdigit(*temp2); i++)			// wyodrebnienie NR_REJESTRU do buffer2
				{
					buffer2[i] = *temp2;
					temp2++;
				}

				hex = intoHex(buffer2, 1);
				strcat(buffer, hex);						// zapis NR_REJESTRU do buffer
				free(hex);

				hex = intoHex(temp1, 4);
				strcat(buffer, hex);						// zapis PRZESUNIECIE do buffer
				free(hex);
			}
			else							// b). pamiec podawana w rozkazie jest w postaci ETYKIETY
			{
				ptr = searchForLabel(buffer2);
				if (ptr == NULL) exit(1);

				temp1 = calloc((MAX_BIAS_LENGTH + 1), sizeof(char));
				if (temp1 == NULL) exit(1);

				sprintf(temp1, "%hu", getRegFromLabel(ptr));			// temp1 wskazuje na powiazany z etykieta NR_REJESTRU

				hex = intoHex(temp1, 1);
				strcat(buffer, hex);									// zapis NR_REJESTRU do buffer
				free(hex);

				sprintf(temp1, "%hu", getBiasFromLabel(ptr));			// temp1 wskazuje na powiazane z etykieta PRZESUNIECIE

				hex = intoHex(temp1, 4);
				strcat(buffer, hex);									// zapis PRZESUNIECIE do buffer
				free(hex);
			}
			free(temp1);
		}
	}


	// 2 BLOK: OBSLUGA ROZKAZOW SKOKU
	else
	{
		strcat(buffer, "0");					// zgodnie z umowa nastepuje wyzerowanie polbajtu nastepujacego bezposrednio po kodzie rozkazu

		for (i = 0; *temp2 != '\0'				// wyodrebnienie do buffer2 ETYKIETY lub PRZESUNIECIA, jezeli adres jest postaci PRZESUNIECIE(nr_rejestru)
			&& *temp2 != '('; i++)
		{
			buffer2[i] = *temp2;
			temp2++;
		}										// na koniec wyodrebnienia temp2 wskazuje na '\0' w przypadku wyodrebnienia ETYKIETY lub '(' w przypadku wyodrebnienia PRZESUNIECIA
		buffer2[i] = '\0';

		temp1 = calloc(MAX_BIAS_LENGTH, sizeof(char));
		if (temp1 == NULL) exit(1);


		if (*temp2 == '(')			// a). pamiec podawana w rozkazie jest w formacie PRZESUNIECIE(NR_REJESTRU)
		{
			if (strchr(ordArgs, 41) == NULL) exit(1);		// jesli nie domknieto nawiasu przy podawaniu argumentu przerwij program z kodem 1

			if (strcmp(ordLabel, "") != 0)					// jesli rozkaz posiada etykiete to zapisuje ja wraz z adresem jaki reprezentuje
				saveLabelAsAddress(ordLabel, 14, (unsigned short)strlen((char*)getFromRegistry(14)) / 2, "");

			strcpy(temp1, buffer2);							// temp1 wskazuje na wyodrebnione z rozkazu PRZESUNIECIE
			memset(buffer2, 0, (MAX_LABEL_LENGTH + 1) * sizeof(char));

			temp2++;
			for (i = 0; isdigit(*temp2); i++)				// wyodrebnienie NR_REJESTRU do buffer2
			{
				buffer2[i] = *temp2;
				temp2++;
			}
			buffer2[i] = '\0';

			hex = intoHex(buffer2, 1);
			strcat(buffer, hex);							// zapis NR_REJESTRU do buffer
			free(hex);

			hex = intoHex(temp1, 4);
			strcat(buffer, hex);							// zapis PRZESUNIECIE do buffer
			free(hex);
		}

		else						// b). pamiec podawana w rozkazie jest w postaci ETYKIETY
		{
			if (isdigit(*ordArgs)) exit(1);					// zle wczytano format adresu, czego powodem jest brak znaku '(' w rozkazie (blad w pliku wejsciowym), co zostalo zinterpretowane jako etykieta, podczas gdy jest on w rzeczywistosci przesunieciem z nr rejestru

			saveLabelAsAddress(ordLabel, 14, (unsigned short)strlen((char*)getFromRegistry(14)) / 2, buffer2); // (!)wykorzystuje strukture labelledCommand (ktora normalnie przechowuje wylacznie etykiete i odpowiadajace jej nr rejestru i przesuniecie), aby przechowac nieuzyta wczesniej w programie etykiete, ktora zostala podana w argumencie analizowanego rozkazu. W strukturze przechowywane sa takze wartosci nr rejestru i przesuniecia odpowiadajace analizowanemu rozkazowi (aby umozliwic pozniejsze odwolanie sie do niego w celu aktualizacji jego kodu).

			ptr = searchForLabel(buffer2);

			if (ptr != NULL)				// znaleziono etykiete do ktorej odwoluje sie rozkaz skoku
			{
				sprintf(temp1, "%hu", getRegFromLabel(ptr)); // temp1 wskazuje teraz na powiazany z etykieta NR_REJESTRU

				hex = intoHex(temp1, 1);
				strcat(buffer, hex);						 // zapis NR_REJESTRU do buffer
				free(hex);

				sprintf(temp1, "%hu", getBiasFromLabel(ptr));// temp1 wskazuje teraz na powiazane z etykieta PRZESUNIECIE

				hex = intoHex(temp1, 4);
				strcat(buffer, hex);						 // zapis PRZESUNIECIE do buffer
				free(hex);
			}
			else {							// nie znaleziono takiej etykiety
				strcat(buffer, "~~~~~");					 // (!) w tym wypadku, gdy rozkaz odwoluje sie do etykiety, ktora nie zostala jeszcze uzyta w programie, odpowiednia funkcjonalnosc funkcji saveLabelAsAddress dopisze w miejsce tyld odpowiedni kod kiedy po raz pierwszy w programie zostanie uzyta ta etykieta dla rozkazu.
			}
		}
		free(temp1);
	}

	storeInDirectiveSection(buffer);
}

void giveOrdCode(char* buff, char* ordSign)
{
	unsigned short i;

	for (i = 0; i != 18; i++)
	{
		if (strcmp(ordSign, List[i].sign) == 0) // przeszukiwanie listy ordCode
		{
			strcat(buff, List[i].code);			// zapis dopasowanego kodu rozkazu do bufora
			return;
		}
	}
	exit(1);									// w razie nieznalezienia odpowiedniego rozkazu zwroc blad i przerwij program
}
char* intoHex(char* dec, int hexBitAmount)
{
	char* word = calloc((hexBitAmount + 1), sizeof(char));
	if (word == NULL) exit(1);

	switch (hexBitAmount)
	{
	case 8:
		sprintf(word, "%08lX", atol(dec));
		break;
	case 4:
		sprintf(word, "%04X", atoi(dec));
		break;
	case 2:
		sprintf(word, "%02X", atoi(dec));
		break;
	case 1:
		sprintf(word, "%01X", atoi(dec));
		break;
	}
	return word;
}
char* deleteSpaces(char* string)
{
	char* temp1;
	char* temp2;
	char* bufferLong; // bufor roboczy

	temp2 = string;
	bufferLong = calloc(DEFAULT + 1, sizeof(char));
	if (bufferLong == NULL) exit(1);

	while (*temp2 != '\0')
	{
		temp1 = strchr(temp2, 32);			//zwraca do temp1 wskaznik na znaleziony znak spacji (' ') lub NULL jesli nie zostal on znaleziony w stringu temp2
		if (temp1 == NULL) temp1 = "";		//jesli nie znaleziono zadnej spacji w ciagu temp1 ma wskazywac na pusty napis, zeby nastepnie prawidlowo przekopiowac caly napis do bufora
		strncat(bufferLong, temp2, strlen(temp2) - strlen(temp1)); //wycina czesc stringa, na ktorego wskazuje temp2, do pierwszego napotkanego znaku spacji
		while (*temp1 == ' ')
			temp1++;						//przesuwa wskaznik temp1 na pierwszy napotkany znak niebedacy znakiem spacji
		temp2 = temp1;						//temp2 wskazuje teraz na nowy ciag znakow zaczynajacy sie od znaku niebedacego znakiem spacji lub na znak '\0' jesli w petli powyzej w ciagu znakow, na ktory wskazywal temp1 nie znaleziono juz zadnego znaku niebedacego spacja 
	}

	strcpy(string, bufferLong);				//na koniec, kiedy nie wykryto juz zadnych spacji w ciagu, nie ma juz czego "doklejac" do bufora wiec mozna skopiowac jego zawartosc (jest to napis wejsciowy niezawierajacy znakow spacji) do ciagu znakow, na ktory wskazuje wejsciowy wskaznik string
	free(bufferLong);

	return string;
}
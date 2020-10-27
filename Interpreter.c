#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
					// PROJEKT POSIADA PE£N¥ DOKUMENTACJÊ FUNKCJI ORAZ WYJAŒNIENIE NIEKTÓRYCH U¯YWANYCH ZMIENNYCH \\
		// ABY ODCZYTAÆ INFORMACJÊ NA TEMAT KTÓREGOŒ Z POWY¯SZYCH, NALE¯Y NAJECHAÆ KURSOREM NA KTÓREŒ Z JEGO WYST¥PIEÑ W KODZIE \\

char* directiveSection;
char* dataSection;
void* registry[] = { 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static long long dataSectionCellsTaken = 0;
static long long directiveSectionCellsTaken = 0;
static long long maxDataSectionCellsToTake = 255;
static long long maxDirectiveSectionCellsToTake = 255;

/**
* Odleglosc znaku '*' od nadanej w dyrektywie o postaci
... <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) do liczby_calkowitej 
*/
int N = 9; 
int i, j, k;
char z;
char word[9];
char emptyWord8[9] = { '~', '~',  '~',  '~',  '~',  '~',  '~',  '~', '\0' };
char* temp1;
char* temp2;
char buffer[255];
char buffer2[255];
struct ordCode 
{
	char* sign;
	char* code;
} List[] = {
	"AR",	"10",
	"A",	"d1",
	"SR",	"12",
	"S",	"d3",
	"MR",	"14", 
	"M",	"d5", 
	"DR",	"16",
	"D",	"d7",
	"CR",	"18",
	"C",	"d9",
	"J",	"e0",
	"JZ",	"e1", 
	"JP",	"e2", 
	"JN",	"e3",
	"L",	"f0",
	"LR",	"f1",
	"LA",	"f2",
	"ST",	"f3",
};
struct ordCode* ptr;

/**
* Symuluje pamiec operacyjna komputera: 
-> alokuje pamiec dla sekcji danych i sekcji sterujacej
-> wpisuje do rejestru 14 adres poczatku sekcji danych 
-> wpisuje do rejestru 15 adres poczatku sekcji sterujacej
*/
void symulateMemory();
/**
* Re-alokuje wiecej pamieci (o 255 znakow) na sekcje danych w razie przepelnienia.
*/
void reallocDataSection();
/**
* Re-alokuje wiecej pamieci (o 255 znakow) na sekcje sterujaca w razie przepelnienia.
*/
void reallocDirectiveSection();
/**
* Zapisuje do sekcji danych nowa wartosc. 
Adres i-tego elementu w sekcji = adres poczatku sekcji + i-te przesuniecie
*/
void storeInDataSection(const char*);
/**
* Zapisuje do sekcji sterujacej nowa wartosc.
Adres i-tego elementu w sekcji = adres poczatku sekcji + i-te przesuniecie
*/
void storeInDirectiveSection(const char*);
/**
* Wyjmuje wartosc znajdujaca sie w i-tym rejestrze.
*/
void* getFromRegistry(int);
/**
* Interpretuje dyrektywe rezerwacji pamieci z jezyka pseudoassemblera na kod maszynowy.
***NARAZIE BEZ ETYKIET***
*/
void interpretDiv(const char* div[]);
/**
* Interpretuje rozkaz (operacji arytmetycznej, przesylania danych lub skoku) 
z jezyka pseudoassemblera na kod maszynowy.
***NARAZIE BEZ ETYKIET***
***TYLKO REJESTR_REJESTR ORAZ KODY ROZKAZU***
*/
void interpretOrd(const char* ord[]);
/**
* Nadaje rozkazowi kod rozkazu. Przyporz¹dkowanie rozkazu do jego kodu znajduje sie w strukturze ordCode,
a zbior wszystkich tych przyporzadkowan znajduje sie w tablicy List.
*/
void giveOrdCode(const char*);
/**
* Zamienia ciag znakow zawierajacy liczbe decymalna 
na ciag znakow reprezentujacy te liczbe heksadecymalnie.
*/
char* intoHex(const char*, int);

void symulateMemory()
{
	dataSection = calloc(255, sizeof(char));
	directiveSection = calloc(255, sizeof(char));
	registry[14] = dataSection;
	registry[15] = directiveSection;
}
void reallocDataSection() 
{
	maxDataSectionCellsToTake += 255;
	registry[14] = realloc(registry[14], (size_t)maxDataSectionCellsToTake);
	printf("DataMemory has been successfully reallocated to %lld\n", maxDataSectionCellsToTake);
}
void reallocDirectiveSection()
{
	maxDirectiveSectionCellsToTake += 255;
	registry[15] = realloc(registry[15], (size_t)maxDirectiveSectionCellsToTake);
	printf("DataMemory has been successfully reallocated to %lld\n", maxDirectiveSectionCellsToTake);
}
void storeInDataSection(const char* val)
{
	i = strlen(val);
	if (dataSectionCellsTaken >= maxDataSectionCellsToTake) 
	{
		reallocDataSection();
		storeInDataSection(val);
	}
	else
	{
		strcat_s(dataSection, 9, val);
		dataSection += i;
		dataSectionCellsTaken += i;
	}
}
void storeInDirectiveSection(const char* val)
{
	i = strlen(val);
	if (directiveSectionCellsTaken >= maxDirectiveSectionCellsToTake)
	{
		reallocDirectiveSection();
		storeInDirectiveSection(val);
	}
	else
	{
		strcat_s(directiveSection, 9, val);
		directiveSection += i;
		directiveSectionCellsTaken += i;
	}
}

void* getFromRegistry(int which)
{
	return registry[which];
}
void interpretDiv(const char* div[]) {
	temp1 = strchr(div[2], 42); //zwraca wskaznik na pozycje gdzie znajduje sie znak '*' (kod ASCII = 42), lub wartosc NULL jesli nie wystepuje
	if (temp1 == NULL)
	{
		if (strcmp(div[1], "DC") == 0)		/* obsluga dyrektyw postaci ... DC INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, div[2] + N - 1, strlen(div[2]) - N); // wycina fragment <liczba_calkowita> do buffer
			temp2 = intoHex(buffer, 8);
			storeInDataSection(temp2);
			printf_s("%s\n", temp2);			//tutaj bedzie zapis do pliku
			memset(buffer, 0, 255 * sizeof(char)); // czysci tablice buffer
		}
		else					/* obsluga dyrektyw postaci ... DS INTEGER */
		{
			storeInDataSection(emptyWord8);
			printf_s("%s\n", emptyWord8);		//tutaj bedzie zapis do pliku
		}
	}
	else 
	{
		strncat_s(buffer, 255, div[2], strlen(div[2]) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> do buffer
		k = atoi(buffer); // liczba k reprezentuje liczbe_komorek_pamieci
		memset(buffer, 0, 255 * sizeof(char));

		if (strcmp(div[1], "DC") == 0)		/* obsluga dyrektyw postaci ... DC <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, (temp1 + N), strlen(temp1) - N - 1); /* wycina fragment <liczba_calkowita> do buffer*/
			for (j = 0; j < k; j++)
			{
				temp2 = intoHex(buffer, 8);
				storeInDataSection(temp2);
				printf_s("%s\n", temp2);		//tutaj bedzie zapis do pliku
			}
			memset(buffer, 0, 255 * sizeof(char));
		}
		else					/* obsluga dyrektyw postaci ... DS <liczba_komorek_pamieci>*INTEGER */
		{
			for (j = 0; j < k; j++)
			{
				storeInDataSection(emptyWord8);
				printf_s("%s\n", emptyWord8);	//tutaj bedzie zapis do pliku
			}
		}
	}
}
void interpretOrd(const char* ord[]) {
	giveOrdCode(ord[1]);		// nadanie do buffer kodu rozkazu
	temp1 = strchr(ord[2], 44); //zwraca wskaznik na pozycje gdzie znajduje sie znak ',' (kod ASCII = 44), lub wartosc NULL jesli nie wystepuje					
	temp2 = ord[2];
	if (temp1 != NULL) // obsluga rozkazow arytmetycznych i zapisywania wartosci 
	{
		if (strlen(ord[1]) == 2 && *(ord[1] + 1) == 'R') // obsluga rozkazow typu rejestr - rejestr
		{
			for (i = 0; *temp2 != ','; i++)
			{
				buffer2[i] = *temp2;
				temp2++;
			}
			strcat_s(buffer, 4, intoHex(buffer2, 1)); // wyodrêbnienie pierwszego rejestru do buffer
			memset(buffer2, 0, 255 * sizeof(char)); 

			temp2++;
			for (i = 0; *temp2 != '\0'; i++)
			{
				buffer2[i] = *temp2;
				temp2++;
			}
			strcat_s(buffer, 5, intoHex(buffer2, 1)); // wyodrêbnienie drugiego rejestru do buffer
			memset(buffer2, 0, 255 * sizeof(char));

			storeInDirectiveSection(buffer);
			printf("%s", buffer);			//tutaj bedzie zapis do pliku
		}
		else // obsluga rozkazow typu rejestr - pamiec
		{

			printf("%s", buffer);			//tutaj bedzie zapis do pliku
		}
	}
	else // obsluga rozkazow skoku
	{

		printf("%s", buffer);				//tutaj bedzie zapis do pliku
	}
	memset(buffer, 0, 255 * sizeof(char));
	temp2 = temp1 = NULL;
}

void giveOrdCode(const char* ordSign)
{
	ptr = List;
	for (;;) // zakladamy ze dane sa prawidlowe i kod zostanie dopasowany przed wyjsciem poza tablice
	{
		if (strcmp(ordSign, ptr->sign) == 0)
		{
			strcat_s(buffer, 3, ptr->code);
			break;
		}
		ptr++;
	}
}
char* intoHex(const char* dec, int hexBitAmount) 
{												
	switch (hexBitAmount)
	{
	case 8:
		sprintf_s(word, 9, "%08llx", atoll(dec));
		break;
	case 4:
		sprintf_s(word, 5, "%04x", atoi(dec));
		break;
	case 2:
		sprintf_s(word, 3, "%02x", atoi(dec));
		break;
	case 1:
		sprintf_s(word, 2, "%01x", atoi(dec));
		break;
	}
	return word;
}
#include "stdtools.h"
#include "memory.h"

/**
* Ilosc pojedynczych komorek pamieci MOZLIWYCH aktualnie do zajecia przez dane w sekcji danych (przy obecnie zaalokowanej pamieci).
* Komorke pamieci rozumiem tutaj jako jeden znak (char) reprezentujacy pojedyncza cyfre szesnastkowa.
*/
unsigned short maxDataSectionCellsToTake;
/**
* Ilosc pojedynczych komorek pamieci MOZLIWYCH aktualnie do zajecia przez dane rozkazow w sekcji rozkazow (przy obecnie zaalokowanej pamieci).
* Komorke pamieci rozumiem tutaj jako jeden znak (char) reprezentujacy pojedyncza cyfre szesnastkowa.
*/
unsigned short maxDirectiveSectionCellsToTake;
/**
* Tablica rejestrow numerowanych od 0 do 15, mogaca przechowywac wartosci typu long int.
*/
long int registry[16];

void symulateMemory()
{
	registry[15] = (long int)calloc(DEFAULT, sizeof(char));
	registry[14] = (long int)calloc(DEFAULT, sizeof(char));

	firstLabelCommAddress = (struct labelledCommand**)malloc((DEFAULT/5) * sizeof(struct labelledCommand*));

	labelledCommandsExecuted = 0;
	maxDataSectionCellsToTake = DEFAULT;
	maxDirectiveSectionCellsToTake = DEFAULT;
	maxLabelledCommandsToExecute = DEFAULT/5;
}
void reallocDataSection()
{
	void* tempAlloc;

	maxDataSectionCellsToTake += DEFAULT;
	tempAlloc = realloc((char*)registry[15], (size_t)maxDataSectionCellsToTake * sizeof(char));
	if (tempAlloc == NULL) exit(100);
	registry[15] = (long int)tempAlloc;
}
void reallocDirectiveSection()
{
	void* tempAlloc;

	maxDirectiveSectionCellsToTake += DEFAULT;
	tempAlloc = realloc((char*)registry[14], (size_t)maxDirectiveSectionCellsToTake * sizeof(char));
	if (tempAlloc == NULL) exit(100);
	registry[14] = (long int)tempAlloc;
}
void reallocLabelledCommands()
{
	struct labelledCommand** ptrToRealloc;

	maxLabelledCommandsToExecute += DEFAULT/10;
	ptrToRealloc = (struct labelledCommand**)realloc(firstLabelCommAddress, (size_t)maxLabelledCommandsToExecute * sizeof(struct labelledCommand*));
	if (ptrToRealloc == NULL) exit(100);
	firstLabelCommAddress = ptrToRealloc;
}
void freeMemory(char* p1, FILE* p2)
{
	struct labelledCommand** ptr;

	for (ptr = firstLabelCommAddress; ptr != firstLabelCommAddress + labelledCommandsExecuted; ptr++)
	{
		free(*ptr);
	}
	free(firstLabelCommAddress);
	free((char*)registry[14]);
	free((char*)registry[15]);
	free(p1);
	free(p2);
}

void storeInDataSection(char* val)
{
	unsigned int j;
	char* section;

	section = (char*)getFromRegistry(15);
	j = strlen(val);											// zmienna j przechowuje dlugosc napisu przekazywanego w argumencie
	if (strlen(section) + j >= maxDataSectionCellsToTake)		// sprawdza czy doszlo do przepelnienia
	{
		reallocDataSection();
		section = (char*)getFromRegistry(15);
	}

	section += strlen(section);									// ustawienie wskaznika do zapisu na ostatni wolny adres w sekcji
	strcat(section, val);										// wlasciwe dodanie wartosci argumentu val do sekcji danych
}
void storeInDirectiveSection(char* val)
{
	unsigned int j;
	char* section;

	section = (char*)getFromRegistry(14);
	j = strlen(val);											// zmienna j przechowuje dlugosc napisu przekazywanego w argumencie
	if (strlen(section) + j >= maxDirectiveSectionCellsToTake)	// sprawdza czy doszlo do przepelnienia
	{
		reallocDirectiveSection();
		section = (char*)getFromRegistry(14);
	}

	section += strlen(section);									// ustawienie wskaznika do zapisu na ostatni wolny adres w sekcji
	strcat(section, val);										// wlasciwe dodanie kodu rozkazu w argumentu val do sekcji rozkazow
}
void modifyData(char* oldDataAddress, char* newData)
{
	unsigned int i;

	for (i = 0; i < (int)strlen(newData); i++)
	{
		*(oldDataAddress + i) = *(newData + i);
	}
}
char* getStringFromSection(char* stringAddress, int dataLength)
{
	char* word;

	word = calloc(MAX_CODE_LENGTH + 1, sizeof(char));
	if (word == NULL) exit(10);

	if ((int)strlen(stringAddress) < dataLength) // zabezpieczenie na wypadek gdyby pod podanym adresem nie znajdowalaby sie rzadana liczba znakow
	{
		strncat(word, stringAddress, strlen(stringAddress));
	}
	else
	{
		strncat(word, stringAddress, dataLength);
	}

	return word;
}
long int getFromRegistry(int which)
{
	return registry[which];
}
void setRegistryVal(int which, long int val)
{
	registry[which] = val;
};
#include "stdtools.h"
#include "memory.h"

/**
* Tablica rejestrow numerowanych od 0 do 15, mogaca przechowywac wartosci typu long int.
*/
long int registry[16];
unsigned long labelledCommandsExecuted = 0;
unsigned short maxDataSectionCellsToTake = DEFAULT;
unsigned short maxDirectiveSectionCellsToTake = DEFAULT;
unsigned long maxLabelledCommandsToExecute = DEFAULT;

void symulateMemory()
{
	registry[15] = (long int)calloc(DEFAULT, sizeof(char));
	registry[14] = (long int)calloc(DEFAULT, sizeof(char));

	firstLabelCommAddress = (struct labelledCommand**)calloc(DEFAULT, sizeof(struct labelledCommand*));
	ptrToSaveLabel = firstLabelCommAddress; 	// inicjacja dodatkowego wskaznika do wprowadzania nowych etykiet
}
void reallocDataSection()
{
	void* tempAlloc;

	maxDataSectionCellsToTake += DEFAULT;
	tempAlloc = realloc((char*)registry[15], (size_t)maxDataSectionCellsToTake);
	if (tempAlloc == NULL) exit(1);
	registry[15] = (long int)tempAlloc;
}
void reallocDirectiveSection()
{
	void* tempAlloc;

	maxDirectiveSectionCellsToTake += DEFAULT;
	tempAlloc = realloc((char*)registry[14], (size_t)maxDirectiveSectionCellsToTake);
	if (tempAlloc == NULL) exit(1);
	registry[14] = (long int)tempAlloc;
}
void reallocLabelledCommands()
{
	struct labelledCommand** ptrToRealloc;

	maxLabelledCommandsToExecute += DEFAULT;
	ptrToRealloc = (struct labelledCommand**)realloc(firstLabelCommAddress, (size_t)maxLabelledCommandsToExecute);
	if (ptrToRealloc == NULL) exit(1);
	firstLabelCommAddress = ptrToRealloc;
	ptrToSaveLabel = firstLabelCommAddress + labelledCommandsExecuted;
}
void freeMemory()
{
	struct labelledCommand** ptr;

	for (ptr = firstLabelCommAddress; ptr != ptrToSaveLabel; ptr++)
	{
		free(*ptr);
	}
	free(firstLabelCommAddress);
	free((char*)registry[14]);
	free((char*)registry[15]);
}

void modifyData(char* oldDataAddress, char* newData)
{
	unsigned int i;

	for (i = 0; i < (int)strlen(newData); i++)
	{
		*(oldDataAddress + i) = *(newData + i);
	}
}
long int getFromRegistry(int which)
{
	return registry[which];
}
void setRegistryVal(int which, long int val)
{
	registry[which] = val;
};
char* getStringFromSection(char* stringAddress, int dataLength)
{
	char* word;

	word = calloc((MAX_CODE_LENGTH + 1), sizeof(char));
	if (word == NULL) exit(1);

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
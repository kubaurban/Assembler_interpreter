#include "stdtools.h"
#include "memory.h"

/**
* Tablica rejestrow numerowanych od 0 do 15, mogaca przechowywac wartosci typu long int.
*/
long int registry[16];
unsigned long labelledCommandsExecuted = 0;
unsigned short maxDataSectionCellsToTake = BYTE_LENGTH;
unsigned short maxDirectiveSectionCellsToTake = BYTE_LENGTH;
unsigned long maxLabelledCommandsToExecute = BYTE_LENGTH;

void symulateMemory()
{
	registry[15] = (long int)calloc(BYTE_LENGTH, sizeof(char));
	registry[14] = (long int)calloc(BYTE_LENGTH, sizeof(char));
	directiveSection = (char*)registry[14];
	dataSection = (char*)registry[15];

	firstLabelCommAddress = (struct labelledCommand**)calloc(BYTE_LENGTH, sizeof(struct labelledCommand*));
	ptrToSaveLabel = firstLabelCommAddress; 	// inicjacja dodatkowego wskaznika do wprowadzania nowych etykiet
}
void reallocDataSection()
{
	void* tempAlloc;

	maxDataSectionCellsToTake += BYTE_LENGTH;
	tempAlloc = realloc((char*)registry[15], (size_t)maxDataSectionCellsToTake);
	if (tempAlloc == NULL) exit(1);
	registry[15] = (long int)tempAlloc;
	dataSection = (char*)registry[15] + strlen((char*)registry[15]);
}
void reallocDirectiveSection()
{
	void* tempAlloc;

	maxDirectiveSectionCellsToTake += BYTE_LENGTH;
	tempAlloc = realloc((char*)registry[14], (size_t)maxDirectiveSectionCellsToTake);
	if (tempAlloc == NULL) exit(1);
	registry[14] = (long int)tempAlloc;
	directiveSection = (char*)registry[14] + strlen((char*)registry[14]);
}
void reallocLabelledCommands()
{
	struct labelledCommand** ptrToRealloc;

	maxLabelledCommandsToExecute += BYTE_LENGTH;
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
#include "stdtools.h"
#include "memory.h"

/**
* Tablica rejestrow numerowanych od 0 do 15, mogaca przechowywac wartosci typu long int lub wskazniki (adresy).
*/
union reg registry[16];
unsigned long labelledCommandsExecuted = 0;
unsigned short maxDataSectionCellsToTake = BYTE_LENGTH;
unsigned short maxDirectiveSectionCellsToTake = BYTE_LENGTH;
unsigned long maxLabelledCommandsToExecute = BYTE_LENGTH;

void symulateMemory()
{
	registry[15].pval = calloc(BYTE_LENGTH, sizeof(char));
	registry[14].pval = calloc(BYTE_LENGTH, sizeof(char));
	directiveSection = (char*)registry[14].pval;
	dataSection = (char*)registry[15].pval;

	firstLabelCommAddress = (struct labelledCommand**) calloc(BYTE_LENGTH, sizeof(struct labelledCommand*));
	ptrToSaveLabel = firstLabelCommAddress; 	// inicjacja dodatkowego wskaznika do wprowadzania nowych etykiet
}
void reallocDataSection()
{
	void* tempAlloc;

	maxDataSectionCellsToTake += BYTE_LENGTH;
	tempAlloc = realloc(registry[15].pval, (size_t)maxDataSectionCellsToTake);
	if (tempAlloc == NULL) exit(1);
	registry[15].pval = tempAlloc;
	dataSection = (char*)registry[15].pval + strlen((char*)registry[15].pval);
}
void reallocDirectiveSection()
{
	void* tempAlloc;

	maxDirectiveSectionCellsToTake += BYTE_LENGTH;
	tempAlloc = realloc(registry[14].pval, (size_t)maxDirectiveSectionCellsToTake);
	if (tempAlloc == NULL) exit(1);
	registry[14].pval = tempAlloc;
	directiveSection = (char*)registry[14].pval + strlen((char*)registry[14].pval);
}
void reallocLabelledCommands()
{
	struct labelledCommand** ptrToRealloc;

	maxLabelledCommandsToExecute += BYTE_LENGTH;
	ptrToRealloc = (struct labelledCommand**) realloc(firstLabelCommAddress, (size_t)maxLabelledCommandsToExecute);
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
	free(registry[14].pval);
	free(registry[15].pval);
}

void modifyData(char* oldDataAddress, char* newData)
{
	unsigned int i;

	for (i = 0; i < (int) strlen(newData); i++)
	{
		*(oldDataAddress + i) = *(newData + i);
	}
}
union reg getFromRegistry(int which)
{
	return registry[which];
}
void setRegistryIval(long int val, int which)
{
	registry[which].ival = val;
};
void setRegistryPval(void* val, int which) 
{
	registry[which].pval = val;
};
char* getStringFromSection(char* cellsFromSectionAddressBegining, int dataLength)
{
	char* word;

	word = calloc((MAX_CODE_LENGTH + 1), sizeof(char));
	if (word == NULL) exit(1);

	strncat(word, cellsFromSectionAddressBegining, dataLength);

	return word;
}
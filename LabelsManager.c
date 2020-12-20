#include "stdtools.h"
#include "labelcom.h"
#include "memory.h"
#include <stdio.h>

void saveLabelAsAddress(char label[], int registryNumber, unsigned short bias, char labelArgument[])
{
	struct labelledCommand** ptrArgument, ** ptrToSaveLabel;

	if (labelledCommandsExecuted + 1 >= maxLabelledCommandsToExecute)//sprawdz czy nie doszlo do przepelnienia
		reallocLabelledCommands();

	ptrArgument = NULL;
	if (*label != '\0')
		ptrArgument = searchForLabelArgument(label, firstLabelCommAddress);// wyszukuje wsrod dotychczasowo wykonanych komend podanej etykiety (ale zapisanej jako ARGUMENT). Drugi argument funkcji to wskaznik, od ktorego nalezy rozpoczac przeszukiwanie struktury.

	while (ptrArgument != NULL)											//podmiana jesli etykieta tego rozkazu byla uzyta juz wczesniej jako argument: najpierw uzupelnienie kodu rozkazu w sekcji rozkazow, potem aktualizacja nr rejestru i przesuniecia dla etykiety obecnie interpretowanego rozkazu
	{
		fillEmptyAddressInOrderCode(*ptrArgument, registryNumber, bias);
		ptrArgument = searchForLabelArgument(label, ptrArgument + 1);			//tym razem drugi argument funkcji (wskaznik, od ktorego nalezy rozpoczac przeszukiwanie struktury) to nastepny element wystepujacy po ostatnio znalezionym.
	}

	ptrToSaveLabel = firstLabelCommAddress + labelledCommandsExecuted; //ustawienie wskaznika na nastepna wolna pamiec zarezerwowana na obiekt typu labelledCommand
	*ptrToSaveLabel = calloc(1, sizeof(struct labelledCommand));//zaalokuj pamiec na kolejny obiekt labelledCommand i zwroc na niego aktualny wskaznik
	if (*ptrToSaveLabel == NULL) exit(1);

	strcpy((*ptrToSaveLabel)->label, label);					//nadaj nowemu obiektowi przekazana w funkcji etykiete "label"
	(*ptrToSaveLabel)->registryNumber = registryNumber;			//nadaj nowemu obiektowi przekazany w funkcji nr rejestru "registry"
	(*ptrToSaveLabel)->bias = bias;								//nadaj nowemu obiektowi przekazane w funkcji przesuniecie "bias"
	strcpy((*ptrToSaveLabel)->labelArgument, labelArgument);	//nadaj nowemu obiektowi przekazana w funkcji etykiete labelArgument bedaca argumentem rozkazu

	labelledCommandsExecuted++;
}
void fillEmptyAddressInOrderCode(struct labelledCommand* ptr, int registryNumber, unsigned short bias)
{
	char newData[MAX_CODE_LENGTH - 3 + 1];
	char tempData[MAX_CODE_LENGTH - 4 + 1];

	sprintf(newData, "%01X", registryNumber);
	sprintf(tempData, "%04X", bias);
	strcat(newData, tempData);
	modifyData((ptr->bias) * 2 + 3 + (char*)getFromRegistry(ptr->registryNumber), newData); //pierwszy argument funkcji to adres rozkazu (przesuniety o kod rozkazu i nr 1 rejestru - stad +3), ktory odwolywal sie (po etykiecie) do obecnie interpretowanego rozkazu zanim ten zostal jeszcze wykonany. Wykonanie funkcji aktualizuje nieznana wczesniej czesc kodu rozkazu na adres analizowanego obecnie rozkazu, do ktorego sie odwolywal (przez etykiete w argumencie).
}

unsigned short getRegFromLabel(struct labelledCommand* ptr)
{
	return ptr->registryNumber;
}
unsigned short getBiasFromLabel(struct labelledCommand* ptr)
{
	return ptr->bias;
}

struct labelledCommand* searchForLabel(char* label)
{
	struct labelledCommand** ptrToSearch;

	for (ptrToSearch = firstLabelCommAddress; ptrToSearch != firstLabelCommAddress + labelledCommandsExecuted; ptrToSearch++)	//ptrToSearch wskazuje na adres pierwszej etykietowanej komendy przechowywanej w pamieci, na ktora wskazuje firsLabelCommAddress
	{
		if (strcmp(label, (*ptrToSearch)->label) == 0)	//przegladanie pamieci w celu znalezienia odpowiedniej etykiety komendy
		{
			return *ptrToSearch;
		}
	}
	return NULL;
}
struct labelledCommand** searchForLabelArgument(char* label, struct labelledCommand** ptrStart)
{
	struct labelledCommand** ptrToSearch;

	for (ptrToSearch = ptrStart; ptrToSearch != firstLabelCommAddress + labelledCommandsExecuted; ptrToSearch++)	//ptrToSearch najpierw wskazuje na adres pierwszej (wzgledem podanego w funkcji argumentu) etykietowanej komendy przechowywanej w pamieci
	{
		if (strcmp(label, (*ptrToSearch)->labelArgument) == 0)					//przegladanie pamieci w celu znalezienia odpowiedniej etykiety bedacej argumentem komendy
		{
			return ptrToSearch; // jesli znaleziono etykiete uzyta wczesniej jako argument zwraca wskaznik dokladnie na obiekt labelledCommend, ktory przechowuje szukana etykiete jako argument 
		}
	}
	return NULL;
}
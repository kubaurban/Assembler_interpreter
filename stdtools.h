#include <stdlib.h>
#include <string.h>
#pragma warning(disable:6031) // ignoruj blad niewykorzystania zwracanej wartosci przez funkcje (tutaj scanf lub sscanf)
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


/**
* Zamienia ciag znakow zawierajacy liczbe decymalna na ciag znakow reprezentujacy te liczbe heksadecymalnie.
Argument hexBitAmount to liczba znakow szesnastkowych jaka spodziewamy sie otrzymac na wyjsciu.
*/
char* intoHex(char*, int);
/**
* Usuwa spacje ze stringa i zwraca na niego wskaznik.
*/
char* deleteSpaces(char*);
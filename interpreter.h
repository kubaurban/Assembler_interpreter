#include "iotools.h"
#include <ctype.h>

/**
* Struktura pozwalajaca przechowywac pare znak_rozkazu - kod_rozkazu.
*/
struct ordCode
{
	char sign[3];
	char code[3];
};

/**
* Interpretuje dyrektywe rezerwacji pamieci z jezyka pseudoassemblera na kod maszynowy:
-> zapisuje dana do sekcji danych/rezerwuje w sekcji danych pamiec na dana
-> zapisuje dana w kodzie maszynowym do pliku.
*/
void interpretDiv(char[], char*, char*);
/**
* Interpretuje rozkaz (operacji arytmetycznej, przesylania danych lub skoku)
z jezyka pseudoassemblera na kod maszynowy. Zapisuje rozkaz w kodzie maszynowym do pliku.
*/
void interpretOrd(char[], char*, char*);
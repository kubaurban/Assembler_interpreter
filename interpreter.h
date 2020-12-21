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
* Interpretuje dyrektywe rezerwacji pamieci z jezyka pseudoassemblera na kod maszynowy i zapisuje dana 
* do sekcji danych/rezerwuje w sekcji danych pamiec na dana.
*/
void interpretDiv(char[], char*, char*);
/**
* Interpretuje rozkaz (operacji arytmetycznej, przesylania danych lub skoku)
* z jezyka pseudoasemblera na kod maszynowy i zapisuje go w sekcji rozkazow.
*/
void interpretOrd(char[], char*, char*);
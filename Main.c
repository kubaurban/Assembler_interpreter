#include "stdtools.h"
#include "interpreter.h"
#include "executer.h"

		//					PROJEKT POSIADA PE£N¥ DOKUMENTACJÊ FUNKCJI ORAZ WYJAŒNIENIE U¯YWANYCH STA£YCH					\\
	//		ABY ODCZYTAÆ INFORMACJÊ NA TEMAT KTÓREGOŒ Z POWY¯SZYCH, PROSZÊ NAJECHAÆ KURSOREM NA JEGO WYST¥PIENIE W KODZIE.		\\
//						WYŒWIETLI SIÊ WÓWCZAS OKIENKO Z DEKLARACJ¥, A TU¯ NAD NI¥ OPIS, O ILE ZOSTA£ ON DODANY						\\

main(int argc, char* argv[])
{
FILE* fp;
char* source, * temp;

symulateMemory();

fp = malloc(sizeof(FILE));
if (fp == NULL) exit(10);
source = calloc(MAX_LINE_LENGTH, sizeof(char));
if (source == NULL) exit(10);

fp = getInput(fp, source, argc, argv);

executeProgram();

fp = fopen(strcat(source, "_out.txt"), "w");
saveDataSection(fp);
saveDirectiveSection(fp);
fclose(fp);

freeMemory(source, fp);

return 0;
}


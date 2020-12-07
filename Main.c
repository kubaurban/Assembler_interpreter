#include "stdtools.h"
#include "interpreter.h"
#include "executer.h"

		//					PROJEKT POSIADA PE£N¥ DOKUMENTACJÊ FUNKCJI ORAZ WYJAŒNIENIE U¯YWANYCH STA£YCH					\\
	//		ABY ODCZYTAÆ INFORMACJÊ NA TEMAT KTÓREGOŒ Z POWY¯SZYCH, PROSZÊ NAJECHAÆ KURSOREM NA JEGO WYST¥PIENIE W KODZIE.		\\
//						 WYŒWIETI SIÊ WÓWCZAS OKIENKO Z DEKLARACJ¥, A TU¯ NAD NI¥ OPIS, O ILE ZOSTA£ ON DODANY						\\

main(int argc, char* argv[])
{
	FILE* fp;
	char* source;

	symulateMemory();

	fp = malloc(sizeof(FILE));
	if (fp == NULL) exit(1);
	source = calloc((BYTE_LENGTH + 5), sizeof(char));
	if (source == NULL) exit(1);

	fp = getInput(fp, source, argc, argv);
	fclose(fp);

	executeProgram();
	
	fp = fopen(strcat(source, "_out.txt"), "w");
	saveDataSection(fp);
	saveDirectiveSection(fp);
	fclose(fp);

	freeMemory();
	free(source);
	free(fp);

	return 0;
}
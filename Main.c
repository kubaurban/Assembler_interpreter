#include "stdtools.h"
#include "interpreter.h"
#include "executer.h"

		//					PROJEKT POSIADA PE�N� DOKUMENTACJ� FUNKCJI ORAZ WYJA�NIENIE U�YWANYCH STA�YCH					\\
	//		ABY ODCZYTA� INFORMACJ� NA TEMAT KT�REGO� Z POWY�SZYCH, PROSZ� NAJECHA� KURSOREM NA JEGO WYST�PIENIE W KODZIE.		\\
//						WY�WIETLI SI� W�WCZAS OKIENKO Z DEKLARACJ�, A TU� NAD NI� OPIS, O ILE ZOSTA� ON DODANY						\\

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


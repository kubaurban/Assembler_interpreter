#include <stdio.h>
#include <string.h>

const char* div1[] = { "dziewiec",	"DC",	"INTEGER(4294967295)" };
const char* div2[] = { "piec",		"DC",	"INTEGER(0)" };
const char* div3[] = { "trzy",		"DC",	"INTEGER(3)" };
const char* div4[] = { "wektor",	"DC",	"15*INTEGER(4294967295)" };
const char* div5[] = { "wynik",		"DS",	"INTEGER" };
const char* div6[] = { "wynik_wek",	"DS",	"5*INTEGER" };

const char* ord1[] = { "r1",	 "L",	"2,dziewiec" };
const char* ord2[] = { "r2",	 "S",	"2,piec" };
const char* ord3[] = { "r3",	 "S",	"2,trzy" };
const char* ord4[] = { "r4",	 "ST",	"2,wynik" };
const char* ord5[] = { "r5",	 "SR",	"2,2" };
const char* ord6[] = { "r6",	 "JN",	"r1" };

void symulateMemory();
void* getFromRegistry(int);
void interpretDiv(const char*[]);
void interpretOrd(const char*[]);
main(int argc, char** argv)
{
	symulateMemory();

	interpretDiv(div1);
	interpretDiv(div2);
	interpretDiv(div3);
	interpretDiv(div4);
	interpretDiv(div5);
	interpretDiv(div6);
	printf("%s", "\n");

	interpretOrd(ord5);
	interpretOrd(ord5);

	
	printf("%s", "\n\n");
	
	printf("Sekcja danych: %s\n", getFromRegistry(14));
	printf("Sekcja sterujaca: %s\n", getFromRegistry(15));
}


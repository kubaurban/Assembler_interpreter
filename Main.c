#include <stdio.h>
#include <string.h>
#define DATASECTION_REGISTRY = "0000000f";
#define ORDERSSECTION_REGISTRY = "0000000e";

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
const char* ord5[] = { "r5",	 "L",	"2,3" };
const char* ord6[] = { "r6",	 "JN",	"r1" };


void interpret_div(const char*[]);
void interpret_ord(const char*[]);
main(int argc, char** argv)
{
	char* dsp; // data_section_pointer start
	char* osp; // order_section_pointer start

	interpret_div(div1);
	interpret_div(div2);
	interpret_div(div3);
	interpret_div(div4);
	interpret_div(div5);
	interpret_div(div6);
	printf("%s", "\n");
	interpret_ord(ord5);

	
	printf("%s", "\n");
}


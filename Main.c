#include <stdio.h>
#include <string.h>

const char* div1[] = { "dziewiec",	"DC",	"INTEGER(9)" };
const char* div2[] = { "piec",		"DC",	"INTEGER(5)" };
const char* div3[] = { "trzy",		"DC",	"INTEGER(3)" };
const char* div4[] = { "wektor",	"DC",	"15*INTEGER(200)" };
const char* div5[] = { "wynik",		"DS",	"INTEGER" };
const char* div6[] = { "wynik_wek",	"DS",	"8*INTEGER" };

const char* ord1[] = { "r1",	 "L",	"2",	"dziewiec" };
const char* ord2[] = { "r2",	 "S",	"2",	"piec" };
const char* ord3[] = { "r3",	 "S",	"2",	"trzy" };
const char* ord4[] = { "r4",	 "ST",	"2",	"wynik" };



void interpret_div(const char*[]);
main(int argc, char** argv)
{
	interpret_div(div1);
	interpret_div(div2);
	interpret_div(div3);
	interpret_div(div4);
	interpret_div(div5);
	interpret_div(div6);
}

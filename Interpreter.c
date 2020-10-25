#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int N = 9; /*odleglosc znaku '*' od nadanej w dyrektywie o postaci
				... <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) do liczby_calkowitej */
int i, j, k;
char word1[9];
char word2[9] = { '0','0','0','0','0','0','0','0','\0' };
char emptyWord[9] = { '~', '~',  '~',  '~',  '~',  '~',  '~',  '~', '\0' };
char* temp1;
char buffer[255];
char t1[255];


char* into_hex(const char*);
void interpret_div(const char* div[]) {
	strncat_s(t1, 255, div[2], 1); // wycina pierwszy znak z ostatniego segmentu instrukcji i zapisuje do t1
	if (strcmp(div[1], "DC") == 0)
	{
		if (!isdigit(*t1)) /* obsluga dyrektyw postaci ... DC INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, div[2] + N - 1, strlen(div[2]) - N);
			printf_s("%s\n", into_hex(buffer)); //tutaj zapisz do pliku 
			memset(buffer, 0, 255 * sizeof(char));
		}
		else /* obsluga dyrektyw postaci ... DC <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) */
		{
			temp1 = strchr(div[2], 42); // zwraca wskaznik na pozycje gdzie znajduje sie znak '*' (kod ASCII = 42) 
			strncat_s(buffer, 255, div[2], strlen(div[2]) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> i zapisuje do buffer
			k = atoi(buffer); // liczba k reprezentuje liczbe_komorek_pamieci
			memset(buffer, 0, 255 * sizeof(char));
			strncat_s(buffer, 255, (temp1 + N), strlen(temp1) - N - 1); /* wycina fragment <liczba_calkowita> i zapisuje do buffer*/
			for (j = 0; j < k; j++) 
			{
				printf_s("%s\n", into_hex(buffer)); //tutaj zapisz do pliku
			}
			memset(buffer, 0, 255 * sizeof(char));
		}
	}
	else 
	{
		if (!isdigit(*t1)) /* obsluga dyrektyw postaci ... DS INTEGER */
		{
			printf_s("%s\n", emptyWord); //tutaj zapisz do pliku 
		}
		else /* obsluga dyrektyw postaci ... DS <liczba_komorek_pamieci>*INTEGER */
		{
			temp1 = strchr(div[2], 42); // zwraca wskaznik na pozycje gdzie znajduje sie znak '*' (kod ASCII = 42) 
			strncat_s(buffer, 255, div[2], strlen(div[2]) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> i zapisuje do buffer
			k = atoi(buffer); // liczba k reprezentuje liczbe_komorek_pamieci
			memset(buffer, 0, 255 * sizeof(char));
			for (j = 0; j < k; j++)
			{
				printf_s("%s\n", emptyWord); //tutaj zapisz do pliku
			}
		}
	}
	memset(t1, 0, 255 * sizeof(char));
}

char* interpret_ord(char** ord) {
	char* bin;
	enum order_codes ord_code;
}

char* into_hex(const char* dec)
{
	sprintf_s(word1, 8, "%x", atoi(dec));
	for (i = 0; i < strlen(word1); i++)
	{
		word2[7 - i] = word1[strlen(word1) - i - 1];
	}
	return word2;
}
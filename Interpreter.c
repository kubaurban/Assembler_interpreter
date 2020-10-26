#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int N = 9; /*odleglosc znaku '*' od nadanej w dyrektywie o postaci
				... <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) do liczby_calkowitej */
int i, j, k;
char word[9];
char word8[9] = { '0','0','0','0','0','0','0','0','\0' };
char word4[5] = { '0','0','0','0','\0' };
char emptyWord[9] = { '~', '~',  '~',  '~',  '~',  '~',  '~',  '~', '\0' };
char* temp1;
char* temp2;
char buffer[255];


char* into_hex(const char*, int);
void interpret_div(const char* div[]) {
	temp1 = strchr(div[2], 42); //zwraca wskaznik na pozycje gdzie znajduje sie znak '*' (kod ASCII = 42), lub wartosc NULL jesli nie wystepuje
	if (strcmp(div[1], "DC") == 0)
	{
		if (temp1 == NULL)		/* obsluga dyrektyw postaci ... DC INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, div[2] + N - 1, strlen(div[2]) - N);
			printf_s("%s\n", into_hex(buffer, 8)); //tutaj zapisz do pliku 
			memset(buffer, 0, 255 * sizeof(char));
		}

		else					/* obsluga dyrektyw postaci ... DC <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, div[2], strlen(div[2]) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> i zapisuje do buffer
			k = atoi(buffer); // liczba k reprezentuje liczbe_komorek_pamieci
			memset(buffer, 0, 255 * sizeof(char));
			strncat_s(buffer, 255, (temp1 + N), strlen(temp1) - N - 1); /* wycina fragment <liczba_calkowita> i zapisuje do buffer*/
			for (j = 0; j < k; j++) 
			{
				printf_s("%s\n", into_hex(buffer, 8)); //tutaj zapisz do pliku
			}
			memset(buffer, 0, 255 * sizeof(char));
		}
	}

	else 
	{
		if (temp1 == NULL)		/* obsluga dyrektyw postaci ... DS INTEGER */
		{
			printf_s("%s\n", emptyWord); //tutaj zapisz do pliku 
		}

		else					/* obsluga dyrektyw postaci ... DS <liczba_komorek_pamieci>*INTEGER */
		{
			strncat_s(buffer, 255, div[2], strlen(div[2]) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> i zapisuje do buffer
			k = atoi(buffer); // liczba k reprezentuje liczbe_komorek_pamieci
			memset(buffer, 0, 255 * sizeof(char));
			for (j = 0; j < k; j++)
			{
				printf_s("%s\n", emptyWord); //tutaj zapisz do pliku
			}
		}
	}
}

char* interpret_ord(char** ord) {
	char* bin;
	temp1 = strchr(ord[2], 44); //zwraca wskaznik na pozycje gdzie znajduje sie znak ',' (kod ASCII = 44), lub wartosc NULL jesli nie wystepuje
	// tutaj nastapi nadanie do buffer kodu rozkazu
	if (temp1 != NULL) // obsluga rozkazow arytmetycznych i zapisywania wartosci 
	{
		//strncat_s()
		//if()
	}

	else // obsluga rozkazow skoku
	{

	}
}

char* into_hex(const char* dec, int bit_amount) // zamienia ciag znakow zawierajacy decymalna liczbe na ciag znakow w reprezentujacy te liczbe heksadecymalnie
{
	sprintf_s(word, bit_amount, "%x", atoi(dec));
	if (bit_amount == 8)
	{
		temp2 = word8;
	}
	else
	{
		temp2 = word4;
	}
	for (i = 0; i < strlen(word); i++)
	{
		temp2[bit_amount - 1 - i] = word[strlen(word) - i - 1];
	}
	return temp2;
}
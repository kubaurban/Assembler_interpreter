#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int N = 9; /*odleglosc znaku '*' od nadanej w dyrektywie o postaci
				... <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) do liczby_calkowitej */
int i, j, k;
char z;
char word[9];
char word8[9]; // slowo pamieci 4B - 32 bity, 8 bitów szesnastkowo
char word4[5]; // pamiec 2B - 16 bitów, 4 bity szesnastkowo
char word2[3]; // jeden bajt - 8 bitów, 2 bity szesnastkowo
char word1[2]; // jeden polbajt - 4 bity, 1 bit szesnastkowo
char emptyWord8[9] = { '~', '~',  '~',  '~',  '~',  '~',  '~',  '~', '\0' };
char* temp1;
char* temp2;
char buffer[255];
char buffer2[255];


char* into_hex(const char*, int);
void give_ordCode(const char*);

void interpret_div(const char* div[]) {
	temp1 = strchr(div[2], 42); //zwraca wskaznik na pozycje gdzie znajduje sie znak '*' (kod ASCII = 42), lub wartosc NULL jesli nie wystepuje
	if (temp1 == NULL)
	{
		if (strcmp(div[1], "DC") == 0)		/* obsluga dyrektyw postaci ... DC INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, div[2] + N - 1, strlen(div[2]) - N); // wycina fragment <liczba_komorek_pamieci> i zapisuje do buffer
			printf_s("%s\n", into_hex(buffer, 8)); //tutaj zapisz do pliku 
			memset(buffer, 0, 255 * sizeof(char)); // czysci tablice buffer
		}
		else								/* obsluga dyrektyw postaci ... DS INTEGER */
		{
			printf_s("%s\n", emptyWord8); //tutaj zapisz do pliku 
		}
	}
	else 
	{
		strncat_s(buffer, 255, div[2], strlen(div[2]) - strlen(temp1)); // wycina fragment <liczba_komorek_pamieci> i zapisuje do buffer
		k = atoi(buffer); // liczba k reprezentuje liczbe_komorek_pamieci
		memset(buffer, 0, 255 * sizeof(char));

		if (strcmp(div[1], "DC") == 0)		/* obsluga dyrektyw postaci ... DC <liczba_komorek_pamieci>*INTEGER(<liczba_calkowita>) */
		{
			strncat_s(buffer, 255, (temp1 + N), strlen(temp1) - N - 1); /* wycina fragment <liczba_calkowita> i zapisuje do buffer*/
			for (j = 0; j < k; j++)
			{
				printf_s("%s\n", into_hex(buffer, 8)); //tutaj zapisz do pliku
			}
			memset(buffer, 0, 255 * sizeof(char));
		}
		else								/* obsluga dyrektyw postaci ... DS <liczba_komorek_pamieci>*INTEGER */
		{
			for (j = 0; j < k; j++)
			{
				printf_s("%s\n", emptyWord8); //tutaj zapisz do pliku
			}
		}
	}
}

void interpret_ord(char* ord[]) {
	give_ordCode(ord[1]);		// nadanie do buffer kodu rozkazu

	temp1 = strchr(ord[2], 44); //zwraca wskaznik na pozycje gdzie znajduje sie znak ',' (kod ASCII = 44), lub wartosc NULL jesli nie wystepuje					
	if (temp1 != NULL) // obsluga rozkazow arytmetycznych i zapisywania wartosci 
	{
		if (strlen(ord[1]) == 2 && *(ord[1] + 1) == 'R') // obsluga rozkazow typu rejestr - rejestr
		{
			for (i = 0; (*ord[2]) != ','; i++)
			{
				buffer2[i] = *(ord[2]);
				ord[2]++;
			}
			strcat_s(buffer, 4, into_hex(buffer2, 1)); // wyodrêbnienie pierwszego rejestru
			memset(buffer2, 0, 255 * sizeof(char)); 

			ord[2]++;
			for (i = 0; (*ord[2]) != '\0'; i++)
			{
				buffer2[i] = *(ord[2]);
				ord[2]++;
			}
			strcat_s(buffer, 5, into_hex(buffer2, 1)); // wyodrêbnienie drugiego rejestru
			memset(buffer2, 0, 255 * sizeof(char));

			printf("%s", buffer); // zapisz do pliku
		}
		else // obsluga rozkazow typu rejestr - pamiec
		{
			printf("%s", buffer); // zapisz do pliku

		}
	}

	else // obsluga rozkazow skoku
	{
		printf("%s", buffer); // zapisz do pliku
	}
	memset(buffer, 0, 255 * sizeof(char));
}

void zeruj_slowo(int b, char* slowo)
{
	for (i = 0; i < b; i++)
	{
		slowo[i] = '0';
	}
}
char* into_hex(const char* dec, int hex_bit_amount) // zamienia ciag znakow zawierajacy decymalna liczbe na ciag znakow w reprezentujacy te liczbe heksadecymalnie
{
	sprintf_s(word, hex_bit_amount + 1, "%llx", atoll(dec));
	switch (hex_bit_amount)
	{
		case 8:
			temp2 = word8;
			break;
		case 4:
			temp2 = word4;
			break;
		case 2:
			temp2 = word2;
			break;
		case 1:
			temp2 = word1;
			break;
	}
	zeruj_slowo(hex_bit_amount, temp2);
	for (i = 0; i < strlen(word); i++)
	{
		temp2[hex_bit_amount - 1 - i] = word[strlen(word) - i - 1];
	}
	return temp2;
}
void give_ordCode(const char* ord_sign) 
{
	if (strlen(ord_sign) == 2)
	{
		z = *(ord_sign + 1);
		if (z == 'R')
		{
			z = *ord_sign;
			switch (z)
			{
			case 'A':
				temp2 = into_hex("16", 2); // AR 
				break;
			case 'S':
				temp2 = into_hex("18", 2); // SR
				break;
			case 'M':
				temp2 = into_hex("20", 2); // MR
				break;
			case 'D':
				temp2 = into_hex("22", 2); // DR
				break;
			case 'C':
				temp2 = into_hex("24", 2); // CR
				break;
			default:
				temp2 = into_hex("241", 2); // LR
				break;
			}
		}
		else
		{
			switch (z)
			{
			case 'Z':
				temp2 = into_hex("225", 2); // JZ
				break;
			case 'N':
				temp2 = into_hex("227", 2); // JN
				break;
			case 'P':
				temp2 = into_hex("226", 2); // JP
				break;
			case 'T':
				temp2 = into_hex("243", 2); // ST
				break;
			case 'A':
				temp2 = into_hex("242", 2); // LA
				break;
			default:
				break;
			}
		}
	}
	else
	{
		switch (*ord_sign)
		{
		case 'A':
			temp2 = into_hex("209", 2); // A
			break;
		case 'S':
			temp2 = into_hex("211", 2); // S
			break;
		case 'M':
			temp2 = into_hex("213", 2); // M
			break;
		case 'D':
			temp2 = into_hex("215", 2); // D
			break;
		case 'C':
			temp2 = into_hex("217", 2); // C
			break;
		case 'J':
			temp2 = into_hex("224", 2); // J
			break;
		default:
			temp2 = into_hex("240", 2); // L
			break;
		}
	}
	strcat_s(buffer, 3, temp2);
}
#include "stdtools.h"
#include "executer.h"
#include <ctype.h>
#include <math.h>

/*
* Funkcja jako pierwszy argument przyjmuje rejestr stanu programu. Modyfikuje dane okreslone za pomoca flagi
(1 - czesc adresowa, 0 - znak ostatniej operacji) na podany w trzecim argumencie ciag bitow.
*/
void changeVector(char* vector, int flag, char* newBinaryData);
/*
* Funkcja wykonuje rozkaz o podanej w pierwszym argumencie tresci. Domyslnie przekazywany jest rozkaz o dlugosci 4B,
wiec jesli analizowany rozkaz ma dlugosc 2B to moze zdarzyc sie, ze druga polowa rozkazu jest zbedna.
Nie przeszkadza to jednak w dalszym wykonaniu programu, natomiast upraszcza implementacje funkcji.
*/
unsigned long executeCommand(char* commandAddress, char* PSVector, int* regFlags);
/*
* Zwraca tzw. efektywny adres wyliczany ze wzoru adres efektywny = adres(pobierany z rejestru) + przesuniecie.
* Funkcja w pierwszym argumencie przyjmuje numer rejestru, z ktorego nalezy pobrac adres, a w drugim argumencie
* wartosc przesuniecia w postaci szesnastkowej.
*/
char* getEffectiveAddress(int reg, char* adr);
/*
* Zamienia liczbe podana w drugim argumencie (system dziesietny) na liczbe w systemie dwojkowym - format 32 bitowy.
Otrzymana liczbe binarnie zapisuje w przekazywanej w pierwszym argumencie tablicy.
*/
void decIntoBinary(char bin[], unsigned long int number);
/*
* Zamienia liczbe z dowolnego systemu (nalezy baze tego systemu podac w pierwszym argumencie) na liczbe dziesietna bez znaku.
*/
unsigned long unsignIntoDec(int otherSystem, char* number);
/*
* Zamienia liczbe z dowolnego systemu (nalezy baze tego systemu podac w pierwszym argumencie) na liczbe dziesietna ze znakiem.
*/
long signIntoDec(int otherSystem, char* bin);

void AR(int reg1, int reg2, int* regFlags, char* PSVector);
void A(int reg1, int* regFlags, char* adr, char* PSVector);
void SR(int reg1, int reg2, int* regFlags, char* PSVector);
void S(int reg1, int* regFlags, char* adr, char* PSVector);
void MR(int reg1, int reg2, char* PSVector);
void M(int reg1, char* adr, char* PSVector);
void DR(int reg1, int reg2, char* PSVector);
void D(int reg1, char* adr, char* PSVector);
void CR(int reg1, int reg2, char* PSVector);
void C(int reg1, char* adr, char* PSVector);
unsigned long J(char* adr);
unsigned long JZ(char* adr, char* PSVector);
unsigned long JP(char* adr, char* PSVector);
unsigned long JN(char* adr, char* PSVector);
void LR(int reg1, int reg2);
void L(int reg1, char* adr);
void ST(int reg1, char* adr);
void LA(int reg1, char* adr);

/*
* Funkcja odpowiada za ustawienie odpowiednich bitow rejestru stanu programu reprezentujacych znak ostatniej operacji arytmetycznej.
* (ewentualnie ustawia kombinacje bitow "11" w przypadku wystapienia bledu)
*/
void changeSignAfterArithmeticCommand(long result, char* PSVector, int status);
/*
* Funkcja zwraca 1 jesli wystapilo przepelnienie w operacji arytmetycznej lub 0 w przeciwnym wypadku.
*/
int isOverflow(long long result);


void executeProgram()
{
	char programStatusVector[65];	// 8 bajtowy rejestr stanu programu
	char bin[33];					// bufor na binarna reprezentacje adresu nastepnego rozkazu przechowywanego w rejestrze stanu programu
	char* adr, * cmd;
	unsigned long newAdr;
	int regFlag[16];				// rejestr flag odpowiada kazdemu rejestrowi, zawiera flage informujaca o tym czy w odpowiadajacym rejestrze znajduje sie adres czy wartosc danej

	programStatusVector[64] = bin[32] = '\0';
									
									/*faza wstepna przygotowujaca program do wykonania*/
	memset(regFlag, 0, 16 * sizeof(int));
	regFlag[14] = regFlag[15] = 1;
	memset(programStatusVector, '0', 64);					// wypelnienie zerami rejestru stanu programu
	decIntoBinary(bin, (unsigned)getFromRegistry(14));		// zaladowanie do bufora bin binarnej reprezentacji adresu poczatku sekcji rozkazow umieszczonej w rejestrze 14
	changeVector(programStatusVector, 1, bin);
	adr = (char*)unsignIntoDec(2, programStatusVector + 32);
									
									/*faza wykonywania kolejnych instrukcji programu*/
	while (*adr != '\0')
	{
		cmd = getStringFromSection(adr, 8);
		newAdr = executeCommand(cmd, programStatusVector, regFlag);
		free(cmd);

		decIntoBinary(bin, newAdr);
		changeVector(programStatusVector, 1, bin);				// ustawienie nowego adresu w rejestrze stanu programu
		adr = (char*)unsignIntoDec(2, programStatusVector + 32);// odczytanie adresu nastepnego rozkazu do wykonania z rej. stanu programu 
	}
}
void changeVector(char* vector, int flag, char* newBinaryData)
{
	if (flag == 1) {		// zmiana czesci adresowej
		for (int i = 0; i < 32; i++)
		{
			vector[32 + i] = newBinaryData[i];
			newBinaryData[i] = '0';
		}
	}
	else					// zmiana bitow reprezentujacych znak ostatniej operacji
	{
		vector[16] = *newBinaryData;
		vector[17] = *(newBinaryData + 1);
	}
}
unsigned long executeCommand(char* commandHexAddress, char* PSVector, int *regFlags)
{
	char code[3] = { '0','0' };
	char r1[2], r2[2];
	int reg1, reg2;
	char* data, * effAdr;
	unsigned long newAdr;
	int commandLength = 0;

	r1[1] = r2[1] = code[2] = '\0';
	data = effAdr = NULL;

	strncpy(code, commandHexAddress, 2);	// wyodrebnienie kodu rozkazu

	strncpy(r1, commandHexAddress + 2, 1);	// wyodrebnienie pierwszego rejestru
	reg1 = (int)(unsignIntoDec(16, r1));

	strncpy(r2, commandHexAddress + 3, 1);	// wyodrebnienie drugiego rejestru
	reg2 = (int)(unsignIntoDec(16, r2));

	switch ((int)unsignIntoDec(16, code))
	{
	case 16:
		commandLength = 4;
		AR(reg1, reg2, regFlags, PSVector);
		break;
	case 209:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		data = getStringFromSection(effAdr, 8);
		A(reg1, regFlags, data, PSVector); // drugi argument to argument adresowy (przekazany jako pamiec).
		break;
	case 18:
		commandLength = 4;
		SR(reg1, reg2, regFlags, PSVector);
		break;
	case 211:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		data = getStringFromSection(effAdr, 8);
		S(reg1, regFlags, data, PSVector);
		break;
	case 20:
		commandLength = 4;
		MR(reg1, reg2, PSVector);
		break;
	case 213:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		data = getStringFromSection(effAdr, 8);
		M(reg1, data, PSVector);
		break;
	case 22:
		commandLength = 4;
		DR(reg1, reg2, PSVector);
		break;
	case 215:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		data = getStringFromSection(effAdr, 8);
		D(reg1, data, PSVector);
		break;
	case 24:
		commandLength = 4;
		CR(reg1, reg2, PSVector);
		break;
	case 217:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		data = getStringFromSection(effAdr, 8);
		C(reg1, data, PSVector);
		break;

	case 224:
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		return J(effAdr);
	case 225:
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		return JZ(effAdr, PSVector);
	case 226:
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		return JP(effAdr, PSVector);
	case 227:
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		return JN(effAdr, PSVector);

	case 240:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		data = getStringFromSection(effAdr, 8);
		L(reg1, data);
		regFlags[reg1] = 0;
		break;
	case 49:
		commandLength = 4;
		LR(reg1, reg2);
		regFlags[reg1] = regFlags[reg2];
		break;
	case 242:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		LA(reg1, effAdr);
		regFlags[reg1] = 1;
		break;
	case 243:
		commandLength = 8;
		effAdr = getEffectiveAddress(reg2, commandHexAddress + 4);
		ST(reg1, effAdr);
		break;
	}

	newAdr = unsignIntoDec(2, PSVector + 32) + commandLength; // ustawienie nowego adresu nastepnego rozkazu do wykonania

	free(data);

	return newAdr;
}
char* getEffectiveAddress(int reg, char* adr)
{
	char temp[5] = { '0','0','0','0' };
	unsigned short bias = 0;

	temp[4] = '\0';

	strncpy(temp, adr, 4);
	bias = 2 * (unsigned short)unsignIntoDec(16, temp);

	if (bias >= strlen((char*)getFromRegistry(reg)))
	{
		exit(9);
	}

	return (char*)getFromRegistry(reg) + bias;
}
void decIntoBinary(char bin[], unsigned long int number) //format 32 bitowy
{
	char temp[2];
	int i = 0;

	memset(bin, '0', 32);
	temp[1] = '\0';

	while (number != 0) {
		sprintf(temp, "%d", number % 2);
		number /= 2;
		bin[31 - i] = *temp;
		i++;
	}
}
unsigned long unsignIntoDec(int otherSystem, char* number)
{
	int i = 0;
	int len = strlen(number);
	unsigned long temp = 0;
	char a;

	while (len - i != 0)
	{
		a = number[len - 1 - i];
		if (!isdigit(a))
		{
			temp += (unsigned long)(a - 'A' + 10) * (unsigned long)pow(otherSystem, i);
		}
		else
		{
			temp += (unsigned long)(a - '0') * (unsigned long)pow(otherSystem, i);
		}
		i++;
	}
	return temp;
}
long signIntoDec(int otherSystem, char* number)
{
	int i = 0;
	int len = strlen(number);
	long temp = 0;
	char a;

	while (len - i != 0)
	{
		a = number[len - 1 - i];
		if (!isdigit(a))
		{
			temp += (long)(a - 'A' + 10) * (long)pow(otherSystem, i);
		}
		else
		{
			temp += (long)(a - '0') * (long)pow(otherSystem, i);
		}
		i++;
	}
	return temp;
}

void AR(int reg1, int reg2, int* regFlags, char* PSVector)
{
	long long result;
	int flag = 0;
	long long arg1 = (long long)getFromRegistry(reg1);
	long long arg2 = (long long)getFromRegistry(reg2);

	if (regFlags[reg2] == 1) arg1 *= 2;
	if (regFlags[reg1] == 1) arg2 *= 2;
	result = arg1 + arg2;

	if (!isOverflow(result))
	{
		setRegistryVal(reg1, (long)result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void A(int reg1, int* regFlags, char* adr, char* PSVector)
{
	long long result;
	int flag = 0;
	long long arg2 = (long long)signIntoDec(16, adr);

	if (regFlags[reg1] == 1) arg2 *= 2;
	result = (long long)getFromRegistry(reg1) + arg2;

	if (!isOverflow(result))
	{
		setRegistryVal(reg1, (long)result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void SR(int reg1, int reg2, int* regFlags, char* PSVector)
{
	long long result;
	int flag = 0;
	long long arg2 = (long long)getFromRegistry(reg2);

	if (regFlags[reg1] == 1) arg2 *= 2;
	result = (long long)getFromRegistry(reg1) - arg2;

	if (!isOverflow(result))
	{
		setRegistryVal(reg1, (long)result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void S(int reg1, int* regFlags, char* adr, char* PSVector)
{
	long long result;
	int flag = 0;
	long long arg2 = (long long)signIntoDec(16, adr);

	if (regFlags[reg1] == 1) arg2 *= 2;
	result = (long long)getFromRegistry(reg1) - arg2;

	if (!isOverflow(result))
	{
		setRegistryVal(reg1, (long)result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void MR(int reg1, int reg2, char* PSVector)
{
	long long result;
	int flag = 0;

	result = (long long)getFromRegistry(reg1) * (long long)getFromRegistry(reg2);

	if (!isOverflow(result))
	{
		setRegistryVal(reg1, (long)result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void M(int reg1, char* adr, char* PSVector)
{
	long long result;
	int flag = 0;

	result = (long long)getFromRegistry(reg1) * (long long)signIntoDec(16, adr);

	if (!isOverflow(result))
	{
		setRegistryVal(reg1, (long)result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void DR(int reg1, int reg2, char* PSVector)
{
	long result = 0;
	long arg2;
	int flag = 0;

	if ((arg2 = getFromRegistry(reg2)) != 0)
	{
		result = getFromRegistry(reg1) / arg2;
		setRegistryVal(reg1, result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand(result, PSVector, flag);
}
void D(int reg1, char* adr, char* PSVector)
{
	long result = 0;
	long arg2;
	int flag = 0;

	if ((arg2 = signIntoDec(16, adr)) != 0)
	{
		result = getFromRegistry(reg1) / arg2;
		setRegistryVal(reg1, result);
		flag = 1;
	}

	changeSignAfterArithmeticCommand(result, PSVector, flag);
}
void CR(int reg1, int reg2, char* PSVector)
{
	long long result;
	int flag = 0;

	result = (long long)getFromRegistry(reg1) - (long long)getFromRegistry(reg2);

	if (!isOverflow(result))
		flag = 1;

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
void C(int reg1, char* adr, char* PSVector)
{
	long long result;
	int flag = 0;

	result = (long long)getFromRegistry(reg1) - (long long)signIntoDec(16, adr);

	if (!isOverflow(result))
		flag = 1;

	changeSignAfterArithmeticCommand((long)result, PSVector, flag);
}
unsigned long J(char* adr)
{
	return (unsigned long)adr;
}
unsigned long JZ(char* adr, char* PSVector)
{
	char sign[3];

	strncpy(sign, PSVector + 16, 2);
	sign[2] = '\0';
	if (strcmp(sign, "00") == 0)
	{
		return (unsigned long)adr;
	}
	return unsignIntoDec(2, PSVector + 32) + 8;
}
unsigned long JP(char* adr, char* PSVector)
{
	char sign[3];

	strncpy(sign, PSVector + 16, 2);
	sign[2] = '\0';
	if (strcmp(sign, "01") == 0)
	{
		return (unsigned long)adr;
	}
	return unsignIntoDec(2, PSVector + 32) + 8;
}
unsigned long JN(char* adr, char* PSVector)
{
	char sign[3];

	strncpy(sign, PSVector + 16, 2);
	sign[2] = '\0';
	if (strcmp(sign, "10") == 0)
	{
		return (unsigned long)adr;
	}
	return unsignIntoDec(2, PSVector + 32) + 8;
}
void LR(int reg1, int reg2)
{
	setRegistryVal(reg1, getFromRegistry(reg2));
}
void L(int reg1, char* adr)
{
	setRegistryVal(reg1, signIntoDec(16, adr));
}
void LA(int reg1, char* adr)
{
	setRegistryVal(reg1, (unsigned long)adr);
}
void ST(int reg1, char* adr)
{
	char buff[9];

	buff[8] = '\0';
	sprintf(buff, "%08lX", getFromRegistry(reg1));
	modifyData(adr, buff);
}

void changeSignAfterArithmeticCommand(long result, char* PSVector, int status)
{
	if (status == 0)						// flaga statusu 0 informuje o wystapieniu bledu
	{
		changeVector(PSVector, 0, "11");
		printf("...ExecuteError...");		// sprawdzenie czy wystapil blad w czasie operacji arytmetycznej
	}
	else
	{
		if (result > 0)
		{
			changeVector(PSVector, 0, "01");
		}
		else if (result == 0)
		{
			changeVector(PSVector, 0, "00");
		}
		else
		{
			changeVector(PSVector, 0, "10");
		}
	}
}
int isOverflow(long long result)
{
	if (result > pow(2, 31) - 1 || result < -pow(2, 31))
		return 1;
	return 0;
}
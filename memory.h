/**
*Maksymalna dlugosc ciagu znakow mogaca przechowywac znak komendy.
*/
#define MAX_SIGN_LENGTH 2
/**
*Maksymalna dlugosc ciagu znakow mogaca przechowywac przesuniecie.
Wynosi 6, poniewaz maksymalna wartosc liczby do zapisania na 2B to 65 535 - unsigned short.
*/
#define MAX_BIAS_LENGTH 6
/**
*Maksymalna dlugosc ciagu znakow mogaca przechowywac pelny kod maszynowy jednej komendy.
*/
#define MAX_CODE_LENGTH 8
/**
*Maksymalna dlugosc ciagu znakow mogaca przechowywac etykiete.
*/
#define MAX_LABEL_LENGTH 10
/**
* Maksymalna liczba do zapisania na jednym bajcie (uzyta bez glebszej idei).
*/
#define BYTE_LENGTH 255


/**
* Wskaznik odwolujacy sie do nastepnego "wolnego" adresu w sekcji rozkazow.
*/
char* directiveSection;
/**
* Wskaznik odwolujacy sie do nastepnego "wolnego" adresu w sekcji danych.
*/
char* dataSection;
/**
* Unia reg to odpowiednik pojedynczego rejestru. Moze przechowywac wartosci typu long int lub wskazniki (adresy).
*/
union reg
{
	long int ival;
	void* pval;
};
/**
* Rejestr stanu programu.
*/
char programStatusRegistry[16];
/**
* Ilosc aktualnie wykonanych komend z etykieta.
*/
unsigned long labelledCommandsExecuted;
/**
* Ilosc pojedynczych komorek pamieci MOZLIWYCH aktualnie do zajecia przez dane w sekcji danych (przy obecnie zaalokowanej pamieci).
Komorke pamieci rozumiem tutaj jako jeden znak (char) reprezentujacy pojedyncza cyfre szesnastkowa.
*/
unsigned short maxDataSectionCellsToTake;
/**
* Ilosc pojedynczych komorek pamieci MOZLIWYCH aktualnie do zajecia przez dane rozkazow w sekcji rozkazow (przy obecnie zaalokowanej pamieci).
Komorke pamieci rozumiem tutaj jako jeden znak (char) reprezentujacy pojedyncza cyfre szesnastkowa.
*/
unsigned short maxDirectiveSectionCellsToTake;
/**
* Ilosc komend z etykieta MOZLIWYCH aktualnie do wykonania przez program (przy obecnie zaalokowanej pamieci).
*/
unsigned long maxLabelledCommandsToExecute;
/**
* Struktura wiazaca ETYKIETE nadana komendzie z ADRESEM (danej w przypadku dyrektyw lub rozkazu w przypadku rozkazów).
Adres elementu = adres przechowywany w rejestrze (registryNumber) + przesuniecie (bias)
Tworzone sa odpowiednie wskazniki umozliwiajace dynamiczne zarzadzanie pamiecia oraz wykonywanie operacji na obiektach
labelledCommand (wszystkie wskazuja na wskaznik typu labelledCommand):
-> firstLabelCommAddress zawsze wskazuje na adres pierwszej etykietowanej komendy przechowywanej w pamieci
-> ptrToSaveLabel wskaznik uzywany do zapisu nowej etykietowanej komendy. Zawsze wskazuje na pierwszy wolny adres
	(analog. do dataSection i directiveSection)
*/
struct labelledCommand {
	char label[MAX_LABEL_LENGTH + 1];
	unsigned short registryNumber;
	unsigned short bias;
	char labelArgument[MAX_LABEL_LENGTH + 1];
} **firstLabelCommAddress, ** ptrToSaveLabel;

/**
* Symuluje pamiec operacyjna:
-> alokuje pamiec dla sekcji danych i sekcji sterujacej
-> wpisuje do rejestru 14 adres poczatku sekcji danych
-> wpisuje do rejestru 15 adres poczatku sekcji sterujacej
-> alokuje pamiec na przechowywanie etykiet oraz odpowiadajacych im adresow
*/
void symulateMemory();
/**
* Re-alokuje wiecej pamieci (o 255 znakow) na sekcje danych w razie przepelnienia.
*/
void reallocDataSection();
/**
* Re-alokuje wiecej pamieci (o 255 znakow) na sekcje sterujaca w razie przepelnienia.
*/
void reallocDirectiveSection();
/**
* Re-alokuje wiecej pamieci (o 255 etykietowanych komend) na etykietowane komendy w razie przepelnienia.
*/
void reallocLabelledCommands();
/**
*Zwalnia alokowana podczas trwania programu pamiec: obiekty typu labelledCommand oraz zawartosci rejestrow 14 i 15.
*/
void freeMemory();
/**
*Aktualizuje dane o adresie podanym w pierwszym argumencie na nowe dane podane w drugim argumencie.
*/
void modifyData(char*, char*);
/**
* Zapisuje do sekcji danych nowa wartosc.
Adres i-tego elementu w sekcji = adres poczatku sekcji + i-te przesuniecie
*/
void storeInDataSection(char**, char*);
/**
* Zapisuje do sekcji sterujacej nowy kod rozkazu.
Adres i-tego elementu w sekcji = adres poczatku sekcji + i-te przesuniecie
*/
void storeInDirectiveSection(char**, char*);
/**
* Zwraca wskaznik znajdujacy sie w i-tym rejestrze.
*/
union reg getFromRegistry(int);
/**
* Zapisuje dana liczbowa do i-tego rejestru.
*/
void setRegistryIval(long int, int);
/**
* Zapisuje wskaznik (adres) do i-tego rejestru.
*/
void setRegistryPval(void*, int);
/**
* Wyjmuje wartosc znajdujaca sie w podanej sekcji pod danym adresem, gdzie:
Adres = adres poczatku danej sekcji + przesuniecie
Ostatnim parametrem funkcji jest dlugosc (ilosc znakow) danej, ktora otrzymamy na wyjsciu.
*/
char* getStringFromSection(char*, int);
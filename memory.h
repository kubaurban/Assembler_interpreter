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
* Wartosc domyslna uzyta przy alokacji duzych blokow pamieci.
*/
#define DEFAULT 255


/**
* Ilosc aktualnie wykonanych komend z etykieta.
*/
unsigned long labelledCommandsExecuted;
/**
* Ilosc komend z etykieta MOZLIWYCH aktualnie do wykonania przez program (przy obecnie zaalokowanej pamieci).
*/
unsigned long maxLabelledCommandsToExecute;
/**
* Struktura wiazaca ETYKIETE nadana komendzie z ADRESEM (danej w przypadku dyrektyw lub rozkazu w przypadku rozkazów).
Adres elementu = adres przechowywany w rejestrze (registryNumber) + przesuniecie (bias)
Utworzony jest odpowiedni wskaznik umozliwiajacy dynamiczne zarzadzanie pamiecia oraz wykonywanie operacji na obiektach
labelledCommand: firstLabelCommAddress wskazuje na adres pierwszej etykietowanej komendy przechowywanej w pamieci (analog do zawartosci rej 14 i 15)
*/
struct labelledCommand {
    char label[MAX_LABEL_LENGTH + 1];
    unsigned short registryNumber;
    unsigned short bias;
    char labelArgument[MAX_LABEL_LENGTH + 1];
} **firstLabelCommAddress;

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
* Re-alokuje wiecej pamieci (o 255 znakow) na sekcje rozkazow w razie przepelnienia.
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
* Zapisuje do sekcji danych nowa wartosc podana w argumencie.
Adres i-tego elementu w sekcji = adres poczatku sekcji + i-te przesuniecie
*/
void storeInDataSection(char*);
/**
* Zapisuje do sekcji sterujacej nowy kod rozkazu podany w argumencie.
Adres i-tego elementu w sekcji = adres poczatku sekcji + i-te przesuniecie
*/
void storeInDirectiveSection(char*);
/**
*Aktualizuje dane o adresie podanym w pierwszym argumencie na nowe dane podane w drugim argumencie.
*/
void modifyData(char*, char*);
/**
* Wyjmuje wartosc znajdujaca sie w podanej sekcji pod danym adresem, gdzie:
Adres = adres poczatku danej sekcji + przesuniecie
Ostatnim parametrem funkcji jest dlugosc (ilosc znakow) danej, ktora otrzymamy na wyjsciu.
*/
char* getStringFromSection(char*, int);
/**
* Zwraca wartosc liczbowa znajdujaca sie w i-tym rejestrze.
*/
long int getFromRegistry(int);
/**
* Zapisuje dana liczbowa do i-tego rejestru.
*/
void setRegistryVal(int, long int);
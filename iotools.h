#define MAX_LINE_LENGTH 255


/**
*Wykonuje wszystkie funkcje zwiazane z otwarciem pliku wejsciowego oraz zwraca wskaznik na poprawny plik wejsciowy 
z instrukcjami dla interpretera pseudoasemblera. Zapisuje nazwe pliku wejsciowego (w argumencie source).
*/
FILE* getInput(FILE*, char*, int, char*[]);
/**
*Wczytuje kod z instrukcjami pseudoasemblera z podanego pliku wejsciowego i przekazuje odczytane komendy do interpretera.
*/
void readPAInstructions(FILE*);
/**
*Wczytuje kod maszynowy z podanego pliku wejsciowego i zapisuje do odpowiednich sekcji.
*/
void readMachineCode(FILE* inputf);
/**
*Obsluguje blad zwiazany z blednym otwarciem pliku. Prosi uzytkownika o podanie sciezki wzglednej do pliku 
z instrukcjami pseudoasemblera w przypadku bledu i zwraca ja jako ciag znakow.
*/
char* handleFileOpenError();
/**
* Czyta linie tekstu, na ktora obecnie wskazuje wskaznik na obiekt typu FILE przekazywany w agrumencie.
* Zwraca 1 jesli w strumieniu odczytana zostala pusta linia lub 0 jesli byla niepusta.
*/
int readLineAndCheck(char*, FILE*);

/**
*Zapisuje do pliku sekcje danych w odpowiednim formacie.
*/
void saveDataSection(FILE*);
/**
*Zapisuje do pliku sekcje rozkazow w odpowiednim formacie.
*/
void saveDirectiveSection(FILE*);
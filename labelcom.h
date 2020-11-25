
/**
* Tworzy obiekt struktury labelledCommand wiazacy przekazywana w argumencie etykiete z adresem analizowanej dyrektywy 
lub rozkazu. Ostatni argument jest istotny w kontekscie rozkazow skoku, poniewaz w ich przypadku wczytywana jest do 
niego etykieta przekazywana w argumencie rozkazu skoku. W innych przypadkach przekazywana jest do niego wartosc "".
*/
void saveLabelAsAddress(char[], int, unsigned short, char[]);
/**
* Jako argument otrzymuje wskaznik na obiekt struktury labelledCommand, ktory przechowuje etykiete aktualnie 
analizowanego rozkazu jako ARGUMENT. Wypelnia zlozona z ~ czesc adresu tego rozkazu (ktorego adres znajduje sie 
w obiekcie labelledCommand). 
Uzupelnia je numerem rejestru oraz przesuniecia (podanych jako ostatnie argumenty) analizowanego rozkazu.
*/
void fillEmptyAddressInOrderCode(struct labelledCommand*, int, unsigned short);
/**
* Zwraca numer rejestru, w ktorym znajduje sie dana zwiazana z etykieta w obiekcie labelledCommand, na ktory wskazuje ptr.
*/
unsigned short getRegFromLabel(struct labelledCommand*);
/**
* Zwraca przesuniecie danej zwiazanej z etykieta w obiekcie labelledCommand, na ktory wskazuje ptr.
*/
unsigned short getBiasFromLabel(struct labelledCommand*);
/**
*Wyszukuje wsrod dotychczasowo wykonanych komend podanej etykiety. 
Zwraca wskaznik na obiekt struktury labelledCommand, w ktorym znalazl podana etykiete.
*/
struct labelledCommand* searchForLabel(char*);
/**
* Wyszukuje wsrod dotychczasowo wykonanych komend podanej etykiety (ale zapisanej jako ARGUMENT).
Drugi argument funkcji to wskaznik, od ktorego nalezy rozpoczac przeszukiwanie struktury.
*/
struct labelledCommand** searchForLabelArgument(char*, struct labelledCommand**);

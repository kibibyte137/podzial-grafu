#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 100000  /*Maksymalny rozmiar linii*/

/*Struktura reprezentujaca pojedyncze polaczenie (sasiada)*/
typedef struct Krawedz{
	struct Wezel* sasiad; /*Wskaznik na sasiedni wezel*/
	struct Krawedz* next; /*Nastepne polaczenie w liscie sasiedztwa*/
} Krawedz;

/*Struktura wezla grafu*/
typedef struct Wezel {
    int id; /*Numer id wezla*/
    int x, y; /*Polozenie wezla w grafie*/
    Krawedz* sasiedzi; /*Lista sasiedztwa (wskaznik na pierwszy element)*/
} Wezel;

void dodaj_krawedz(Wezel* a, Wezel* b){
	/*Tworzenie krawedzi od A do B*/
	Krawedz* nowa_krawedz_a = (Krawedz*)malloc(sizeof(Krawedz));
	if(!nowa_krawedz_a){
		perror("Blad alokacji pamieci dla krawedzi grafu.");
		return;
	}
	nowa_krawedz_a->sasiad = b;
	nowa_krawedz_a->next = a->sasiedzi;
	a->sasiedzi = nowa_krawedz_a;
	
	/*Tworzenie krawedzi od B do A (graf nieskierowany)*/
	Krawedz* nowa_krawedz_b = (Krawedz*)malloc(sizeof(Krawedz));
	if(!nowa_krawedz_b){
		perror("Blad alokacji pamieci dla krawedzi grafu.");
		return;
	}
	nowa_krawedz_b->sasiad = a;
	nowa_krawedz_b->next = b->sasiedzi;
	b->sasiedzi = nowa_krawedz_b;
}

/*Funkcja wypisujaca sasiadow wezła */
void wypisz_sasiadow(Wezel* wezel){
	printf("Wezel %d ma sasiadow:", wezel->id);
	Krawedz* k = wezel->sasiedzi;
	while(k){
		printf(" %d", k->sasiad->id);
		k = k->next;
	}
	printf("\n");
}

/*Funkcja zwalniajaca pamiec*/
void zwolnij_wezel(Wezel* wezel){
	Krawedz* k = wezel->sasiedzi;
	while(k){
		Krawedz* temp = k;
		k = k->next;
		free(temp);
	}
	free(wezel);
}

/*Funkcja main programu*/
int main(int argc, char *argv[]){
	
	/*GETOPT*/	
	
	int opt; /*Zmienna przechowujaca opcje getopt*/
	int czesci = 2; /*Zmienna przechowujaca liczbe czesci, na ktora ma zostac podzielony graf podana przez uzytkownika, domyslnie 2*/
	int margines = 10; /*Zmienna przechowujaca margines procentowy ilosci wezlow w poszczegolnych czesciach grafu, domyslnie 10*/
	char *format_wyjsciowy = "txt"; /*Zmienna przechowujaca format pliku wejsciowego podany przez uzytkownika, domyslnie txt*/
	char *nazwa_pliku = NULL; /*Zmienna przechowujaca nazwe pliku podana przez uzytkownika*/
	
	while((opt = getopt(argc, argv, "f:o:c:m:h")) != -1){
		switch (opt) {
			case 'f':
				nazwa_pliku = optarg; /*Pobranie nazwy pliku podanej po -f*/
				break;
			case 'o':
				if (optarg) {
					format_wyjsciowy = optarg; /*Ustawienie formatu wyjsciowego, jezeli zostal podany*/
				}
				break;
			case 'c':
				if (optarg) {
					czesci = atoi(optarg); /*Ustawienie liczby czesci, na ktore ma zostac podzielony graf, jezeli zostala podana*/
				}
				break;
			case 'm':
				if (optarg) {
					margines = atoi(optarg); /*Ustawienie marginesu procentowego ilosci wezlow w poszczegolnych czesciach grafu, jezeli zostal podany*/
				}
				break;
			case 'h':
				// pomoc(); /*Wywolanie funkcji wyswietlajacej podreczna pomoc*/
				printf("Wyswietlenie podrecznej pomocy.\n");
				return 0; /*Zakonczenie programu po wyswietleniu pomocy */
			default:
				printf("Podano nieprawidlowa opcje.\n"); /*Blad dla nieznanych opcji*/
				// pomoc(); /*Wyswietlenie pomocy*/
				printf("Wyswietlenie podrecznej pomocy.\n");
				return 7; /*Zakonczenie programu z bledem*/
		}
	}	
	
	/*OBSLUGA BLEDOW*/
	
	if(format_wyjsciowy != "txt" && format_wyjsciowy != "bin"){
		printf("Podano nieprawidlowy format pliku wyjsciowego: %s.", format_wyjsciowy);
		return 2;
	}
	
	if(margines < 0 || margines > 100){
		printf("Podano niepoprawny margines procentowy: %d.", margines);
		return 3;
	}
	
	/*OBSLUGA BLEDOW PO ODCZYTANIU GRAFU*/
	
	/*Niepoprawny format pliku: Nie udalo sie odczytac pliku: plik. Kod powrotu programu 5*/
	
	/* V - liczba wierzchołków w grafie
	if(czesci < 0 || czesci > V){
		printf("Nie mozna podzielic grafu na podana liczbe czesci: %s.", czesci);
		return 3;
	}
	*/
	
	/*DEBUG*/
	printf("Podany plik to: %s.\n", nazwa_pliku ? nazwa_pliku : "BRAK");
	printf("Przyjety format wyjsciowy to: %s.\n", format_wyjsciowy);
	printf("Przyjeta liczba czesci to: %d.\n", czesci);
	printf("Przyjety margines procentowy to: %d.\n", margines); 
	
	/*WCZYTYWANIE PLIKOW I INTERPRETACJA GRAFOW*/
	
	int m; /*Liczba kolumn w grafie*/
	int n; /*Liczba wierszy w grafie*/
	int v; /*Liczba wezlow w grafie*/
	
	int i; 
	int j; 
	
	char linia[MAX_LINE];
	char linia_wezlow[MAX_LINE];
	char linia_podzialow[MAX_LINE];
	
	int indeks_w_linii = 0;
	
	FILE* plik = fopen(nazwa_pliku, "r");
	if(!plik){
		printf("Nie znaleziono pliku: %s.\n", nazwa_pliku);
		return 1;
	}
	
	if(fgets(linia, sizeof(linia), plik)){
		m = atoi(linia);
		printf("DEBUG: Maksymalna liczba węzłów w wierszu: %d.\n", m);
    }
	
	/*Wczytanie dwoch linii*/
	fgets(linia_wezlow, sizeof(linia), plik);
	fgets(linia_podzialow, sizeof(linia), plik);
	
	/*Tablice do przechowywania wezlow*/
	int wezly[MAX_LINE];
	int indeksy_podzialu[MAX_LINE];
	
	/*Odczytanie indeksow wezlow w wierszu grafu*/
	int liczba_wezlow = 0;
	char* token = strtok(linia_wezlow, ";");
	while(token){
		wezly[liczba_wezlow++] = atoi(token);
		token = strtok(NULL, ";");
	}
	
	/*Odczytanie indeksow podzialu*/
	int liczba_podzialow = 0;
	token = strtok(linia_podzialow, ";");
	while (token) {
		indeksy_podzialu[liczba_podzialow++] = atoi(token);
		token = strtok(NULL, ";");
	}

	/*Przetworzenie danych*/
	int aktualny_indeks = 0;
	printf("Wiersze grafu:\n");

	for (int i = 0; i < liczba_podzialow - 1; i++) {
		printf("Wiersz %d:", i + 1);
		for (int j = indeksy_podzialu[i]; j < indeksy_podzialu[i + 1]; j++) {
			printf(" [%d, %d]", i + 1, wezly[j]);
		}
		printf("\n");
	}
	
	return 0;
}
/*NOTATKA 21.04.2025
Wartości są poprawnie wczytywane do tablic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINIA 100000	/*Maksymalny rozmiar linii*/

typedef struct{
	int max_w_wierszu; /*Maksymalna liczba wezlow w wierszu grafu (nie musi wystapic wiersz z taka iloscia wezlow)*/
	
	int* indeksy; /*Indeksy wezlow w poszczegolnych wierszach*/
	int* indeksy_ptr; /*Wskazniki na pierwsze indeksy węzłów w wierszach*/
	
	int* grupy; /*Grupy wezlow polaczone przy pomocy krawedzi*/
	int* grupy_ptr; /*Wskazniki na pierwsze wezly w poszczegolnych grupach*/
	
	int* stopnie; /*Stopnie poszczegolnych wezlow*/
} MacierzCSR;

typedef struct{
	int n; /*Liczba wezlow*/
	int nnz; /*Liczba niezerowych elementów w macierzy Laplace'a*/
	int* row_ptr; /*Wskazniki na pierwszy element wiersza*/
	int* col_idx; /*Indeksy kolumn (sąsiedzi wierzcholka)*/
	double* values; /*Wartosci macierzy Laplace'a L*/
} MacierzLaplaceCSR;

void oblicz_stopnie(MacierzCSR *graf, int liczba_wezlow) {
	for (int i = 0; i < liczba_wezlow; i++) {
		graf->stopnie[i] = graf->grupy_ptr[i + 1] - graf->grupy_ptr[i];
	}
}

/*Funkcja do wczytywania i konwersji wartosci z pliku*/
int wczytaj_wartosci(FILE* plik, int* tablica){
	char linia[MAX_LINIA];
	if (!fgets(linia, sizeof(linia), plik)) return 0;

	int licznik = 0;
	char* token = strtok(linia, ";");
	while(token){
		tablica[licznik++] = atoi(token);
		token = strtok(NULL, ";");
	}
	return licznik;
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
	printf("DEBUG: Podany plik to: %s.\n", nazwa_pliku ? nazwa_pliku : "BRAK");
	printf("DEBUG: Przyjety format wyjsciowy to: %s.\n", format_wyjsciowy);
	printf("DEBUG: Przyjeta liczba czesci to: %d.\n", czesci);
	printf("DEBUG: Przyjety margines procentowy to: %d.\n", margines); 
	
	MacierzCSR* macierz = malloc(sizeof(MacierzCSR));
	if (!macierz) {
		return -1; /*Blad alokacji pamieci dla struktury*/
	}

	macierz->max_w_wierszu = 0;
	macierz->indeksy = malloc(MAX_LINIA * sizeof(int));
	macierz->indeksy_ptr = malloc(MAX_LINIA * sizeof(int));
	macierz->grupy = malloc(MAX_LINIA * sizeof(int));
	macierz->grupy_ptr = malloc(MAX_LINIA * sizeof(int));

	if (!macierz->indeksy || !macierz->indeksy_ptr || !macierz->grupy || !macierz->grupy_ptr) {
		/*Obsługa bledu alokacji pamieci*/
		free(macierz->indeksy);
		free(macierz->indeksy_ptr);
		free(macierz->grupy);
		free(macierz->grupy_ptr);
		free(macierz); // Zwolnienie struktury
		return -1;
	}

	/*ODCZYTYWANIE PLIKU*/
	char linia[MAX_LINIA];
	
	FILE* plik = fopen(nazwa_pliku, "r");
	if(!plik){
		printf("Nie znaleziono pliku: %s.\n", nazwa_pliku);
		return 1;
	}
	
	if(fgets(linia, sizeof(linia), plik)){
		macierz->max_w_wierszu = atoi(linia);
		printf("DEBUG: Maksymalna liczba wezlow w wierszu: %d.\n", macierz->max_w_wierszu);
	}
	
	int liczba_wezlow = wczytaj_wartosci(plik, macierz->indeksy);
	printf("DEBUG: Liczba wezlow w grafie: %d.\n", liczba_wezlow);
	
	int liczba_wierszy = wczytaj_wartosci(plik, macierz->indeksy_ptr) - 1;
	printf("DEBUG: Liczba wierszy w grafie: %d.\n", liczba_wierszy);
	
	printf("\nDEBUG: Macierz obecnosci\n");
	for(int i = 0; i < liczba_wierszy; i++){
		printf("Wiersz %5d:\t", i);
		for(int j = macierz->indeksy_ptr[i]; j < macierz->indeksy_ptr[i+1]; j++){
			printf("%d ", macierz->indeksy[j]);
		}
		printf("\n");
	}
	printf("\n");
	
	int elementy_grup = wczytaj_wartosci(plik, macierz->grupy);
	printf("DEBUG: Liczba elementow w grupach: %d.\n", elementy_grup);
	
	int liczba_grup = wczytaj_wartosci(plik, macierz->grupy_ptr);
	printf("DEBUG: Liczba grup wezlow: %d.\n", liczba_grup);

	printf("\nDEBUG: Polaczenia miedzy wezlami:\n");
	for (int i = 0; i < liczba_grup; i++) {
		int start = macierz->grupy_ptr[i];
		int end = (i == liczba_grup - 1) ? elementy_grup - 1 : macierz->grupy_ptr[i + 1] - 1;

		printf("Grupa %d: ", i + 1);
		for (int j = start; j <= end; j++) {
			printf("%d ", macierz->grupy[j]);
		}
		printf("\n");
	}
	
	free(macierz->indeksy);
	free(macierz->indeksy_ptr);
	free(macierz->grupy);
	free(macierz->grupy_ptr);
	free(macierz);
	
	fclose(plik);
	return 0;
}

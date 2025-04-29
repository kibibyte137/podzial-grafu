#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "struktury.h"
#include "wczytaj.h"
#include "stworz_graf.h"
#include "operacje_macierzowe.h"
#include "dziel_graf.h"
#include "walidacja.h"
#include "wyniki.h"
#include "pomoc.h"

/*Funkcja main programu*/
int main(int argc, char *argv[]){
	
	/*GETOPT*/	
	
	int opt; /*Zmienna przechowujaca opcje getopt*/
	int czesci = 2; /*Zmienna przechowujaca liczbe czesci, na ktora ma zostac podzielony graf podana przez uzytkownika, domyslnie 2*/
	int margines = 10; /*Zmienna przechowujaca margines procentowy ilosci wezlow w poszczegolnych czesciach grafu, domyslnie 10*/
	char *format_wyjsciowy = "csrrg"; /*Zmienna przechowujaca format pliku wejsciowego podany przez uzytkownika, domyslnie csrrg*/
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
				wyswietl_pomoc(); /*Wywolanie funkcji wyswietlajacej podreczna pomoc*/
				return 0; /*Zakonczenie programu po wyswietleniu pomocy */
			default:
				printf("Podano nieprawidlowa opcje.\n"); /*Blad dla nieznanych opcji*/
				wyswietl_pomoc(); /*Wyswietlenie pomocy*/
				return 7; /*Zakonczenie programu z bledem*/
		}
	}	
	
	/*OBSLUGA BLEDOW*/
	if (strcmp(format_wyjsciowy, "csrrg") != 0 && strcmp(format_wyjsciowy, "bin") != 0) {
		printf("Podano nieprawidlowy format pliku wyjsciowego: %s.\n", format_wyjsciowy);
		wyswietl_pomoc();
		return 2;
	}
	
	if(margines < 0 || margines > 100){
		printf("Podano niepoprawny margines procentowy: %d.\n", margines);
		wyswietl_pomoc();
		return 4;
	}
	
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
		wyswietl_pomoc();
		return 1;
	}
	
	if(fgets(linia, sizeof(linia), plik)){
		macierz->max_w_wierszu = atoi(linia);
		printf("DEBUG: Maksymalna liczba wezlow w wierszu: %d.\n", macierz->max_w_wierszu);
	}
	
	int liczba_wezlow = wczytaj_wartosci(plik, macierz->indeksy);
	printf("DEBUG: Liczba wezlow w grafie: %d.\n", liczba_wezlow);
	
	if(czesci <= 1 || czesci > liczba_wezlow){
		printf("Nie mozna podzielic grafu na podana liczbe czesci: %d.\n", czesci);
		wyswietl_pomoc();
		return 3;
	}
	
	int liczba_wierszy = wczytaj_wartosci(plik, macierz->indeksy_ptr) - 1;
	printf("DEBUG: Liczba wierszy w grafie: %d.\n", liczba_wierszy);
	
	/*
	printf("\nDEBUG: Macierz obecnosci\n");
	for(int i = 0; i < liczba_wierszy; i++){
		printf("Wiersz %5d:\t", i);
		for(int j = macierz->indeksy_ptr[i]; j < macierz->indeksy_ptr[i+1]; j++){
			printf("%d ", macierz->indeksy[j]);
		}
		printf("\n");
	}
	printf("\n");
	*/
	
	int elementy_grup = wczytaj_wartosci(plik, macierz->grupy);
	printf("DEBUG: Liczba elementow w grupach: %d.\n", elementy_grup);
	
	int liczba_grup = wczytaj_wartosci(plik, macierz->grupy_ptr);
	printf("DEBUG: Liczba grup wezlow: %d.\n", liczba_grup);

	/*
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
	*/
	
	// Obliczenie stopni
	macierz->stopnie = malloc(liczba_wezlow * sizeof(int));
	oblicz_stopnie(macierz, liczba_wezlow, liczba_grup, elementy_grup);

	// Tworzenie macierzy Laplace’a
	MacierzLaplaceCSR* L = zbuduj_macierz_Laplace(macierz, liczba_wezlow);

	// Alokacja indeksów i przypisań
	int* przypisania = malloc(liczba_wezlow * sizeof(int));
	int* indeksy = malloc(liczba_wezlow * sizeof(int));
	for (int i = 0; i < liczba_wezlow; i++) indeksy[i] = i;

	// Rekurencyjny podział na k części
	rekurencyjny_podzial(L, indeksy, liczba_wezlow, przypisania, 0, czesci, margines);
	refine_balancing(L, przypisania, liczba_wezlow, czesci, margines);



	// Wyświetlenie wyniku
	printf("\n=== Podział grafu na %d części (margines %d%%) ===\n", czesci, margines);
	for (int i = 0; i < liczba_wezlow; i++) {
		printf("Wierzcholek %d => czesc %d\n", i, przypisania[i]);
	}
	
	/*
	printf("\n=== Stopnie wezlow ===\n");
	for (int i = 0; i < liczba_wezlow; i++) {
		printf("Wierzcholek %d ma stopien: %d\n", i, macierz->stopnie[i]);
	}
	*/

	/*
	printf("\n=== Macierz Laplace'a (CSR) ===\n");
	for (int i = 0; i < liczba_wezlow; i++) {
		printf("Wiersz %d: ", i);
		for (int j = L->row_ptr[i]; j < L->row_ptr[i+1]; j++) {
			printf("(kolumna %d: %f) ", L->col_idx[j], L->values[j]);
		}
		printf("\n");
	}
	*/
	
	// Najpierw sprawdzamy teoretycznie:
	if (!czy_podzial_mozliwy(liczba_wezlow, czesci, margines)) {
		printf("\nUWAGA: Teoretycznie nie da się podzielić grafu na %d części z marginesem %d%%.\n", czesci, margines);
	} else {
		printf("\nUWAGA: Teoretycznie da się podzielić grafu na %d części z marginesem %d%%.\n", czesci, margines);
	}

	// Teraz sprawdzamy rzeczywisty podział:
	printf("\nSprawdzanie poprawnosci rzeczywistego podzialu:\n");
	if (sprawdz_margines(przypisania, liczba_wezlow, czesci, margines)) {
		printf("\nSUKCES: Podzial grafu spelnia margines %d%%.\n", margines);
	} else {
		printf("\nBLAD: Podzial grafu NIE spelnia marginesu %d%%.\n", margines);
	}

	 // Wywołanie funkcji zapisującej do pliku
    char nazwa_wyjsciowa[256];
    
    // Domyślna nazwa pliku wyjściowego
    strcpy(nazwa_wyjsciowa, "wynik.csrrg");
    
    // Jeśli podano flagę -o csrrg, używamy tej nazwy
    if (format_wyjsciowy && strcmp(format_wyjsciowy, "csrrg") == 0) {
        printf("Zapisuję podzielony graf w formacie CSRRG\n");
         // Zawsze zapisujemy wynik (domyślnie "wynik.csrrg")
    zapisz_do_pliku_csrrg(nazwa_wyjsciowa, macierz, przypisania, liczba_wezlow, 
                         czesci, liczba_grup, elementy_grup, liczba_wierszy);
    }
	
    if (strcmp(format_wyjsciowy, "bin") == 0) {
    zapisz_do_pliku_binarnego("wynik.bin", macierz, przypisania, 
                             liczba_wezlow, czesci, liczba_grup, elementy_grup);
}
	// Sprzątanie
	free(przypisania);
	free(indeksy);
	free(macierz->stopnie);
	free(L->row_ptr);
	free(L->col_idx);
	free(L->values);
	free(L);
	
	free(macierz->indeksy);
	free(macierz->indeksy_ptr);
	free(macierz->grupy);
	free(macierz->grupy_ptr);
	free(macierz);

	fclose(plik);
	return 0;
}
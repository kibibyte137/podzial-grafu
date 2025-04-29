#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wczytaj.h"
#include "struktury.h"

/* === WCZYTYWANIE DANYCH === */

/* 
Funkcja: wczytaj_wartosci
Opis:
	Wczytuje pojedynczą linię tekstu z pliku i rozdziela wartości oddzielone średnikami ';'
	konwertując je do liczb całkowitych i zapisując do tablicy.
Parametry:
	plik — wskaźnik do otwartego pliku wejściowego,
	tablica — wskaźnik do tablicy, w której będą zapisane wartości.
Zwraca:
	Liczbę wczytanych elementów.
*/
int wczytaj_wartosci(FILE* plik, int* tablica){
	char linia[MAX_LINIA]; /*Bufor na wczytaną linię*/

	if(!fgets(linia, sizeof(linia), plik)) return 0; /*Próba wczytania linii z pliku*/

	int licznik = 0; /*Licznik elementów*/
	char* token = strtok(linia, ";"); /*Rozdzielanie linii po średniku*/
	
	while(token){
		tablica[licznik++] = atoi(token); /*Konwersja tokena na int i zapis*/
		token = strtok(NULL, ";"); /*Przejście do następnego tokena*/
	}

	return licznik; /*Zwrócenie liczby odczytanych wartości*/
}

/* 
Funkcja: oblicz_stopnie
Opis:
	Oblicza stopnie (liczbę sąsiadów) każdego wierzchołka w grafie CSR
	na podstawie danych grup węzłów.
Parametry:
	graf — wskaźnik do struktury grafu w formacie CSR,
	liczba_wezlow — liczba wierzchołków w grafie,
	liczba_grup — liczba grup w grafie,
	liczba_elementow_grup — liczba wszystkich elementów we wszystkich grupach.
Zwraca:
	Nic (wyniki zapisywane w strukturze grafu).
*/
void oblicz_stopnie(MacierzCSR* graf, int liczba_wezlow, int liczba_grup, int liczba_elementow_grup){
	
	/*Inicjalizacja: ustawienie wszystkich stopni na 0*/
	for(int i = 0; i < liczba_wezlow; i++){
		graf->stopnie[i] = 0;
	}
	
	/* Przejście przez wszystkie grupy węzłów */
	for(int i = 0; i < liczba_grup; i++){
		int start = graf->grupy_ptr[i]; /*Pierwszy element grupy*/
		int end = (i == liczba_grup - 1) ? liczba_elementow_grup - 1 : graf->grupy_ptr[i + 1] - 1; /*Ostatni element grupy*/

		/*Aktualizacja stopnia dla węzła reprezentującego grupę*/
		graf->stopnie[graf->grupy[start]] = end - start;
		
		/*Uzupełnienie: zliczenie wcześniejszych wystąpień tego samego węzła w grupach*/
		for(int j = 0; j < start; j++){
			if(graf->grupy[j] == graf->grupy[start]){
				graf->stopnie[graf->grupy[start]]++;
			}
		}
	}
	
	/*Dodatkowe uzupełnienie: węzły niebędące pierwszym elementem grupy*/
	for(int i = 0; i < liczba_wezlow; i++){
		if(graf->stopnie[i] == 0){
			for(int j = 0; j < liczba_elementow_grup; j++){
				if(graf->grupy[j] == i){
					graf->stopnie[i]++;
				}
			}
		}
	}
}
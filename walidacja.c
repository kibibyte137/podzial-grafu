#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "walidacja.h"

/*WALIDACJA*/

/*
Funkcja: czy_podzial_mozliwy
Opis:
	Sprawdza teoretyczną możliwość podziału grafu na k części przy zadanym marginesie.
	Oblicza idealną wielkość części, a następnie sprawdza, czy cała liczba węzłów n
	mieści się w zakresie dozwolonym przez margines.
Parametry:
	n — liczba wszystkich węzłów w grafie,
	k — liczba części na które planujemy podział,
	margines — dopuszczalny margines odchylenia procentowego rozmiaru części.
Zwraca:
	1 (prawda) — jeśli podział jest teoretycznie możliwy,
	0 (fałsz) — jeśli podział jest teoretycznie niemożliwy.
*/
int czy_podzial_mozliwy(int n, int k, int margines) {
    double ideal = (double)n / k; /*Obliczenie idealnej liczby węzłów na część*/
    int dol = (int)(ideal * (1.0 - margines / 100.0)); /*Dolna granica rozmiaru części*/
    int gor = (int)ceil(ideal * (1.0 + margines / 100.0)); /*Górna granica rozmiaru części*/

	/*Sprawdzenie czy n węzłów mieści się w zakresie [k*dol, k*gor]*/
    if (k * dol <= n && n <= k * gor) {
        return 1; // Podzial Teoretycznie możliwy
    } else {
        return 0; // Podzial teoretycznie niemożliwy
    }
}

/*
Funkcja: sprawdz_margines
Opis:
	Sprawdza rzeczywiste przypisanie węzłów do części pod kątem zgodności z marginesem.
	Dla każdej części weryfikuje, czy liczba przypisanych do niej węzłów mieści się
	w zakresie wyznaczonym przez margines.
	Wypisuje informacje diagnostyczne dla każdej części.
Parametry:
	przypisania — tablica przypisująca każdy węzeł do części,
	liczba_wezlow — liczba wszystkich węzłów grafu,
	liczba_czesci — liczba części w podziale,
	margines — dopuszczalny margines procentowy odchylenia liczebności części.
Zwraca:
	1 (prawda) — jeśli wszystkie części spełniają margines,
	0 (fałsz) — jeśli chociaż jedna część nie spełnia marginesu.
*/
int sprawdz_margines(int* przypisania, int liczba_wezlow, int liczba_czesci, int margines) {
    int* liczniki = calloc(liczba_czesci, sizeof(int)); /*Licznik węzłów przypisanych do każdej części*/

	/*Zliczanie przypisań węzłów do poszczególnych części*/
    for (int i = 0; i < liczba_wezlow; i++) {
        if (przypisania[i] >= 0 && przypisania[i] < liczba_czesci)
            liczniki[przypisania[i]]++;
    }

	/*Wyznaczenie dolnej i górnej granicy akceptowalnej liczby węzłów*/
    int idealna_wielkosc = liczba_wezlow / liczba_czesci;
    int dolna_granica = (int)(idealna_wielkosc * (1.0 - margines / 100.0));
    int gorna_granica = (int)(idealna_wielkosc * (1.0 + margines / 100.0));

    int poprawny = 1; /*Zakładamy, że podział jest poprawny*/

	/*Weryfikacja każdej części osobno*/
    for (int i = 0; i < liczba_czesci; i++) {
        if (liczniki[i] < dolna_granica || liczniki[i] > gorna_granica) {
            printf("Czesc %d ma %d wezlow, poza marginesem [%d, %d]!\n", i, liczniki[i], dolna_granica, gorna_granica);
            poprawny = 0; /*Błąd — część nie spełnia marginesu*/
        } else {
            printf("Czesc %d OK: %d wezlow\n", i, liczniki[i]);
        }
    }

    free(liczniki); /*Zwolnienie pamięci*/
    return poprawny; /*Zwrócenie informacji o poprawności*/
}
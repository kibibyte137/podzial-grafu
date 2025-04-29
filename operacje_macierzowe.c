#include <stdlib.h>
#include <math.h>

#include "operacje_macierzowe.h"
#include "struktury.h"

/* === OPERACJE MACIERZOWE === */

/*
Funkcja: compare_double
Opis:
	Funkcja pomocnicza do sortowania liczb typu double.
	Porównuje dwa elementy typu double.
Parametry:
	a, b — wskaźniki na elementy do porównania.
Zwraca:
	-1 jeśli pierwsze jest mniejsze, 1 jeśli większe, 0 jeśli równe.
*/
int compare_double(const void* a, const void* b){
	double diff = *(double*)a - *(double*)b; /*Oblicz różnicę między elementami*/
	if(diff < 0){
		return -1; /*Pierwszy mniejszy*/
	} else if(diff > 0){ 
		return 1; /*Pierwszy większy*/
	} else { 
		return 0; /*Są równe*/
	}
}

/*
Funkcja: mnoz_Lx
Opis:
	Realizuje mnożenie macierzy Laplace'a L (w formacie CSR) przez wektor x.
	Wynik zapisywany jest do tablicy wynik.
Parametry:
	L — wskaźnik na macierz Laplace'a,
	x — wektor wejściowy,
	wynik — wektor wynikowy (nadpisywany).
Zwraca:
	Zmienia tablicę wynik[].
*/
void mnoz_Lx(MacierzLaplaceCSR* L, double* x, double* wynik){
	for(int i = 0; i < L->n; i++){
		wynik[i] = 0.0; /*Zerujemy wartość wyniku dla wiersza i*/

		/*Sumujemy iloczyny odpowiednich elementów*/
		for(int j = L->row_ptr[i]; j < L->row_ptr[i + 1]; j++){
			wynik[i] += L->values[j] * x[L->col_idx[j]];
		}
	}
}

/*
Funkcja: metoda_potegowa
Opis:
	Stosuje metodę potęgową do przybliżonego wyznaczenia wektora własnego
	macierzy Laplace'a L odpowiadającego drugiej najmniejszej wartości własnej (wektor Fiedlera).
	(Sama metoda nie gwarantuje odnalezienia dokładnie wektora Fiedlera, ale przy Laplace'ie często się udaje.)
Parametry:
	L — wskaźnik na macierz Laplace'a,
	fiedler — tablica wynikowa na przybliżony wektor,
	iteracje — liczba iteracji metody potęgowej.
Zwraca:
	Wypełnia tablicę fiedler[] wynikami wektora.
*/
void metoda_potegowa(MacierzLaplaceCSR* L, double* fiedler, int iteracje){
	/*Alokacja pamięci na wektory robocze*/
	double* x = malloc(L->n * sizeof(double));
	double* y = malloc(L->n * sizeof(double));

	/*Inicjalizacja: wektor x wypełniony jedynkami*/
	for(int i = 0; i < L->n; i++){
		x[i] = 1.0;
	}

	/*Główna pętla iteracyjna metody potęgowej*/
	for(int k = 0; k < iteracje; k++){
		mnoz_Lx(L, x, y); /* Oblicz L*x = y */

		/*Obliczanie normy wektora y (norma euklidesowa)*/
		double norma = 0.0;
		for(int i = 0; i < L->n; i++){
			norma += y[i] * y[i];
		}
		norma = sqrt(norma); /*Pierwiastek z sumy kwadratów*/

		/*Normalizacja wektora y -> nowy x*/
		for(int i = 0; i < L->n; i++){
			x[i] = y[i] / norma;
		}
	}

	/*Po zakończeniu iteracji kopiujemy wynik do tablicy fiedler*/
	for(int i = 0; i < L->n; i++){
		fiedler[i] = x[i];
	}

	/*Zwolnienie pamięci pomocniczej*/
	free(x);
	free(y);
}
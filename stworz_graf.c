#include <stdlib.h>

#include "stworz_graf.h"
#include "struktury.h"

/* === TWORZENIE GRAFU === */

/*
Funkcja: zbuduj_macierz_Laplace
Opis:
	Tworzy macierz Laplace'a grafu w formacie CSR (Compressed Sparse Row),
	na podstawie danych grafu wczytanych do struktury MacierzCSR.
Parametry:
	graf — wskaźnik do grafu w formacie CSR,
	liczba_wezlow — liczba wierzchołków grafu.
Zwraca:
	Wskaźnik do nowo utworzonej macierzy Laplace'a.
*/
MacierzLaplaceCSR* zbuduj_macierz_Laplace(MacierzCSR* graf, int liczba_wezlow){
	/* Alokacja struktury dla macierzy Laplace'a */
	MacierzLaplaceCSR* L = malloc(sizeof(MacierzLaplaceCSR));
	L->n = liczba_wezlow;

	/* Alokacja pamięci na strukturę CSR */
	L->row_ptr = malloc((liczba_wezlow + 1) * sizeof(int));
	L->col_idx = malloc(10 * liczba_wezlow * sizeof(int)); /* Zakładamy więcej miejsca na krawędzie */
	L->values = malloc(10 * liczba_wezlow * sizeof(double));
	L->nnz = 0; /* Liczba niezerowych elementów (na razie 0) */

	int nnz = 0; /* Lokalne zliczanie niezerowych elementów */
	
	/* Budowanie macierzy Laplace'a */
	for(int i = 0; i < liczba_wezlow; i++){
		L->row_ptr[i] = nnz; /* Zapamiętaj gdzie zaczyna się wiersz */

		/* Wstawienie przekątnej (stopień węzła) */
		L->col_idx[nnz] = i;
		L->values[nnz++] = graf->stopnie[i];

		/* Dodanie sąsiedztwa: krawędzie z wagą -1 */
		for(int j = graf->grupy_ptr[i]; j < graf->grupy_ptr[i + 1]; j++){
			int sasiad = graf->grupy[j];
			if(sasiad != i){ /* Pomijamy własny wierzchołek (przekątna już wstawiona) */
				L->col_idx[nnz] = sasiad;
				L->values[nnz++] = -1.0;
			}
		}
	}
	L->row_ptr[liczba_wezlow] = nnz; /* Zakończenie ostatniego wiersza */
	L->nnz = nnz; /* Aktualizacja liczby niezerowych elementów */

	return L; /* Zwrócenie zbudowanej macierzy */
}

/*
Funkcja: stworz_podgraf_Laplace
Opis:
	Tworzy macierz Laplace'a podgrafu wybranego zbioru węzłów,
	na podstawie istniejącej macierzy Laplace'a pełnego grafu.
Parametry:
	L — wskaźnik na macierz Laplace'a pełnego grafu,
	indeksy — tablica indeksów wybranych wierzchołków,
	liczba_wezlow — liczba wybranych wierzchołków.
Zwraca:
	Wskaźnik do nowo utworzonej macierzy podgrafu.
*/
MacierzLaplaceCSR* stworz_podgraf_Laplace(MacierzLaplaceCSR* L, int* indeksy, int liczba_wezlow){
	/*Alokacja nowej struktury dla podgrafu*/
	MacierzLaplaceCSR* nowy = malloc(sizeof(MacierzLaplaceCSR));
	nowy->n = liczba_wezlow;

	/*Alokacja pamięci dla CSR*/
	nowy->row_ptr = malloc((liczba_wezlow + 1) * sizeof(int));
	nowy->col_idx = malloc(L->nnz * sizeof(int)); /*Na zapas tyle co cały graf*/
	nowy->values = malloc(L->nnz * sizeof(double));

	/*Tworzymy mapowanie: stare indeksy -> nowe indeksy*/
	int* stary2nowy = malloc(L->n * sizeof(int));
	for(int i = 0; i < L->n; i++){
		stary2nowy[i] = -1; /*Domyślnie -1 oznacza brak*/
	}
	for(int i = 0; i < liczba_wezlow; i++){
		stary2nowy[indeksy[i]] = i; /*Przypisanie nowego indeksu*/
	}

	int nnz = 0; /*Zliczanie elementów niezerowych*/
	
	/*Przepisywanie macierzy dla podgrafu*/
	for(int i = 0; i < liczba_wezlow; i++){
		int wezel = indeksy[i]; /*Numer węzła w oryginalnym grafie*/
		nowy->row_ptr[i] = nnz; /*Początek nowego wiersza*/

		/*Przeglądanie sąsiadów*/
		for(int j = L->row_ptr[wezel]; j < L->row_ptr[wezel + 1]; j++){
			int kolumna = L->col_idx[j];
			/*Jeśli sąsiad również należy do podgrafu*/
			if(stary2nowy[kolumna] != -1){
				nowy->col_idx[nnz] = stary2nowy[kolumna]; /*Nowy indeks kolumny*/
				nowy->values[nnz] = L->values[j]; /*Przepisanie wartości*/
				nnz++;
			}
		}
	}
	nowy->row_ptr[liczba_wezlow] = nnz; /*Ustawienie końca ostatniego wiersza*/
	nowy->nnz = nnz; /*Zapisanie liczby niezerowych elementów*/

	/*Zwolnienie pamięci tymczasowego mapowania*/
	free(stary2nowy);

	return nowy; /*Zwrócenie nowej macierzy podgrafu*/
}
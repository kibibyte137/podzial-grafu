#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "dziel_graf.h"
#include "operacje_macierzowe.h"
#include "stworz_graf.h"

/*PODZIAL GRAFU*/
/*
Funkcja: rekurencyjny_podzial
Opis:
	Rekurencyjnie dzieli graf na zadaną liczbę części za pomocą spektralnego podejścia
	w oparciu o wektor Fiedlera uzyskany z macierzy Laplace'a. Podział odbywa się na dwie grupy,
	a następnie rekurencyjnie dalej aż do uzyskania wymaganej liczby części.
Parametry:
	L — wskaźnik na macierz Laplace'a grafu (pełny graf),
	indeksy — tablica indeksów wierzchołków do podziału,
	liczba_wezlow — liczba wierzchołków rozważanych w tej rekursji,
	przypisania — tablica wynikowa z przypisaniem wierzchołków do części,
	start_czesci — numer startowy (przesunięcie numeracji części),
	liczba_czesci — liczba części, na jakie ma zostać podzielona bieżąca grupa,
	margines — margines procentowy (tu nieużywany w funkcji bezpośrednio).
Zwraca:
	Modyfikuje tablicę przypisania węzłów do odpowiednich części.
*/
void rekurencyjny_podzial(MacierzLaplaceCSR* L, int* indeksy, int liczba_wezlow, int* przypisania, int start_czesci, int liczba_czesci, int margines) {
	
	/*Jeśli liczba części to 1, przypisz wszystkie wierzchołki do tej części*/
    if (liczba_czesci == 1) {
		for (int i = 0; i < liczba_wezlow; i++) {
            przypisania[indeksy[i]] = start_czesci;
        }
        return;
    }
	
	/*Alokacja pamięci na indeksy węzłów lewej i prawej części*/
	int* lewa = malloc(liczba_wezlow * sizeof(int));
	int* prawa = malloc(liczba_wezlow * sizeof(int));
	int lsize = 0, rsize = 0;

    /*Alokacja pamięci na wektor Fiedlera*/
    double* fiedler = malloc(liczba_wezlow * sizeof(double));

	/*Stworzenie podgrafu odpowiadającego aktualnym węzłom*/
    MacierzLaplaceCSR* podgraf = stworz_podgraf_Laplace(L, indeksy, liczba_wezlow);

	/*Obliczenie wektora Fiedlera metodą potęgową*/
	metoda_potegowa(podgraf, fiedler, 100);

	/*Kopia wektora Fiedlera do sortowania i wyznaczenia progu*/
	double* kopia = malloc(liczba_wezlow * sizeof(double));
	memcpy(kopia, fiedler, liczba_wezlow * sizeof(double));
	qsort(kopia, liczba_wezlow, sizeof(double), compare_double); /*Sortowanie kopii*/

	/*Wyznaczenie podziału na lewą i prawą stronę*/
	int czesci_lewa = liczba_czesci / 2;
	int czesci_prawa = liczba_czesci - czesci_lewa;
	
	/*Wyznaczenie liczby węzłów dla lewej części*/
	int liczba_wezlow_w_lewej = (int)((double)czesci_lewa / (czesci_lewa + czesci_prawa) * liczba_wezlow);
	if (liczba_wezlow_w_lewej == 0) liczba_wezlow_w_lewej = 1; /*Zabezpieczenie przed pustą lewą stroną*/

	/*Wyznaczenie wartości progowej do rozdziału wierzchołków*/
	double prog = (kopia[liczba_wezlow_w_lewej] + kopia[liczba_wezlow_w_lewej - 1]) / 2.0;

	free(kopia); /*Zwolnienie pamięci kopii*/

	/*Przypisanie wierzchołków na podstawie wartości Fiedlera*/
	for (int i = 0; i < liczba_wezlow; i++) {
		if (fiedler[i] < prog)
			lewa[lsize++] = indeksy[i];
		else if (fiedler[i] == prog) {
			if (lsize < liczba_wezlow_w_lewej)
				lewa[lsize++] = indeksy[i];
			else
				prawa[rsize++] = indeksy[i];
		}
		else
			prawa[rsize++] = indeksy[i];
	}

    free(fiedler); /*Zwolnienie pamięci wektora Fiedlera*/

    /*Rekurencyjne podziały lewej i prawej strony*/
    rekurencyjny_podzial(L, lewa, lsize, przypisania, start_czesci, czesci_lewa, margines);
    rekurencyjny_podzial(L, prawa, rsize, przypisania, start_czesci + czesci_lewa, czesci_prawa, margines);

	/*Zwolnienie pamięci pomocniczych struktur*/
    free(lewa);
    free(prawa);
	free(podgraf->row_ptr);
	free(podgraf->col_idx);
	free(podgraf->values);
	free(podgraf);
}

/*
Funkcja: oblicz_edges_inside_outside
Opis:
	Oblicza liczbę krawędzi wychodzących z danego węzła:
	- inside: krawędzie łączące węzeł z innymi węzłami w tej samej części,
	- outside: krawędzie łączące węzeł z węzłami w innych częściach.
Parametry:
	L — wskaźnik na macierz Laplace'a grafu,
	przypisania — tablica przypisań węzłów do części,
	wezel — numer wierzchołka dla którego liczymy,
	inside — wskaźnik do zmiennej wynikowej liczby krawędzi wewnętrznych,
	outside — wskaźnik do zmiennej wynikowej liczby krawędzi zewnętrznych.
Zwraca:
	Uaktualnia zmienne inside i outside.
*/
void oblicz_edges_inside_outside(MacierzLaplaceCSR* L, int* przypisania, int wezel, int* inside, int* outside) {
    *inside = 0;
    *outside = 0;
    int czesc_wezla = przypisania[wezel]; /*Pobranie numeru części, do której należy węzeł*/

	/*Przeglądanie sąsiadów danego węzła w macierzy Laplace'a*/
    for (int j = L->row_ptr[wezel]; j < L->row_ptr[wezel + 1]; j++) {
        int sasiad = L->col_idx[j];
        if (sasiad == wezel) continue; /*Pominięcie przekątnej (połączenie węzła z samym sobą)*/

		/*Jeśli sąsiad należy do tej samej części - inside, inaczej - outside*/
        if (przypisania[sasiad] == czesc_wezla)
            (*inside)++;
        else
            (*outside)++;
    }
}

/*
Funkcja: refine_balancing
Opis:
	Poprawia zbalansowanie liczebności części grafu po wstępnym podziale.
	Jeżeli jakaś część ma za dużo wierzchołków, przenosi najlepsze węzły
	do części niedoborowych, minimalizując jednocześnie liczbę przecięć krawędzi.
Parametry:
	L — wskaźnik na macierz Laplace'a grafu,
	przypisania — tablica przypisań węzłów do części,
	liczba_wezlow — liczba wszystkich węzłów grafu,
	liczba_czesci — liczba części na które podzielono graf,
	margines — procentowy margines tolerancji wielkości części.
Zwraca:
	Modyfikuje tablicę przypisań węzłów do części.
*/
void refine_balancing(MacierzLaplaceCSR* L, int* przypisania, int liczba_wezlow, int liczba_czesci, int margines) {
    int* liczniki = calloc(liczba_czesci, sizeof(int)); /*Liczba węzłów w każdej części*/

	/*Zliczenie liczby węzłów przypisanych do każdej części*/
    for (int i = 0; i < liczba_wezlow; i++) {
        liczniki[przypisania[i]]++;
    }

	/*Obliczenie dolnej i górnej granicy wielkości części zgodnie z marginesem*/
    int idealna_wielkosc = liczba_wezlow / liczba_czesci;
    int dolna = (int)(idealna_wielkosc * (1.0 - margines/100.0));
    int gorna = (int)ceil(idealna_wielkosc * (1.0 + margines/100.0));

    int poprawione; /*Flaga czy wykonano przesunięcie*/
    do {
        poprawione = 0; /*Na początku brak przesunięć*/

		/*Przegląd wszystkich części*/
        for (int i = 0; i < liczba_czesci; i++) {
            if (liczniki[i] > gorna) {
                int najlepszy_wezel = -1;
                int najlepszy_inside = 1e9;
                int najlepszy_outside = -1;
                int najlepsza_nowa_czesc = -1;

                /*Szukaj najlepszego kandydata do przeniesienia z części i*/
                for (int w = 0; w < liczba_wezlow; w++) {
                    if (przypisania[w] != i) continue; /*Wierzchołek nie należy do przepełnionej części*/

                    int inside, outside;
                    oblicz_edges_inside_outside(L, przypisania, w, &inside, &outside);

					/*Próba przypisania do części o niedoborze*/
                    for (int j = 0; j < liczba_czesci; j++) {
                        if (liczniki[j] < dolna) {
							/*Kryterium wyboru: minimalizuj inside, maksymalizuj outside*/
                            if (outside >= inside || liczniki[i] > 2 * gorna) { // Korzystne przesunięcie
                                if (inside < najlepszy_inside || (inside == najlepszy_inside && outside > najlepszy_outside)) {
                                    najlepszy_wezel = w;
                                    najlepszy_inside = inside;
                                    najlepszy_outside = outside;
                                    najlepsza_nowa_czesc = j;
                                }
                            }
                        }
                    }
                }
				/*Jeśli znaleziono odpowiedniego węzła do przeniesienia*/
                if (najlepszy_wezel != -1) {
                    przypisania[najlepszy_wezel] = najlepsza_nowa_czesc;
                    liczniki[i]--;
                    liczniki[najlepsza_nowa_czesc]++;
                    poprawione = 1; /*Wykonano poprawkę*/
                }
            }
        }
    } while (poprawione); /*Kontynuuj, aż brak zmian*/

    free(liczniki); /*Zwolnienie pamięci*/
}
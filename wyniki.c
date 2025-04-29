#include <stdio.h>
#include <stdlib.h>
#include "wyniki.h"

/*ZAPIS WYNIKOW*/

/*
Funkcja: zapisz_do_pliku_csrrg
Opis:
	Zapisuje podzielony graf do pliku tekstowego w formacie CSRRG.
	Zapisuje podstawowe dane grafu oraz informacje o partycjach węzłów.
	Każda część (partycja) ma osobny zestaw wskaźników grup.
Parametry:
	nazwa_pliku — nazwa pliku wynikowego (ścieżka do pliku),
	graf — wskaźnik na strukturę grafu w formacie CSR,
	przypisania — tablica przypisująca węzły do części,
	liczba_wezlow — liczba wszystkich węzłów w grafie,
	liczba_czesci — liczba części na które został podzielony graf,
	liczba_grup — liczba grup węzłów,
	elementy_grup — łączna liczba elementów w tablicy grup,
	liczba_wierszy — liczba wierszy (np. kolumn lub grup) w grafie.
Zwraca:
	Brak zwracanej wartości — zapisuje dane bezpośrednio do pliku.
*/
void zapisz_do_pliku_csrrg(const char* nazwa_pliku, MacierzCSR* graf, int* przypisania, int liczba_wezlow, int liczba_czesci, int liczba_grup, int elementy_grup, int liczba_wierszy) {
    FILE* plik = fopen(nazwa_pliku, "w"); /*Otwarcie pliku do zapisu*/
    if (!plik) {
        printf("Nie można utworzyć pliku wyjściowego: %s\n", nazwa_pliku);
        return;
    }

    /*1. Zapisanie maksymalnej liczby węzłów w wierszu*/
    fprintf(plik, "%d\n", graf->max_w_wierszu);

    /*2. Zapisanie indeksów węzłów w wierszach*/
    for (int i = 0; i < liczba_wezlow; i++) {
        if (i > 0) fprintf(plik, ";");
        fprintf(plik, "%d", graf->indeksy[i]);
    }
    fprintf(plik, "\n");

    /*3. Zapisanie wskaźników na początku wierszy*/
    for (int i = 0; i <= liczba_wierszy; i++) {
        if (i > 0) fprintf(plik, ";");
        fprintf(plik, "%d", graf->indeksy_ptr[i]);
    }
    fprintf(plik, "\n");

    /*4. Zapisanie grup węzłów*/
    for (int i = 0; i < elementy_grup; i++) {
        if (i > 0) fprintf(plik, ";");
        fprintf(plik, "%d", graf->grupy[i]);
    }
    fprintf(plik, "\n");

    /*5. Zapisanie wskaźników grup dla każdej partycji*/
    for (int czesc = 0; czesc < liczba_czesci; czesc++) {
        int licznik = 0;
        fprintf(plik, "0;"); /*Pierwszy wskaźnik zawsze 0*/
        
        for (int g = 0; g < liczba_grup; g++) {
            int start = graf->grupy_ptr[g];
            int end = (g == liczba_grup - 1) ? elementy_grup : graf->grupy_ptr[g+1];
            
            int w_grupie = 0;
            for (int i = start; i < end; i++) {
                int wezel = graf->grupy[i];
                if (przypisania[wezel] == czesc) {
                    w_grupie++;
                }
            }
            licznik += w_grupie;
            fprintf(plik, "%d;", licznik);
        }
        fprintf(plik, "\n");
    }

    fclose(plik); /*Zamknięcie pliku*/
    printf("Zapisano podzielony graf do pliku: %s\n", nazwa_pliku);
}

/*
Funkcja: oblicz_wskazniki_dla_partycji
Opis:
	Dla podanej partycji oblicza tablicę wskaźników do grup krawędzi,
	wskazując ile węzłów należy do danej grupy w tej partycji.
Parametry:
	graf — wskaźnik na strukturę grafu w formacie CSR,
	przypisania — tablica przypisująca węzły do części,
	liczba_wezlow — liczba wszystkich węzłów grafu,
	liczba_grup — liczba grup w grafie,
	elementy_grup — łączna liczba elementów w tablicy grup,
	czesc — numer części dla której wyznaczamy wskaźniki.
Zwraca:
	Wskaźnik na nowo utworzoną tablicę wskaźników (należy zwolnić pamięć po użyciu).
*/
int* oblicz_wskazniki_dla_partycji(MacierzCSR* graf, int* przypisania, int liczba_wezlow, int liczba_grup, int elementy_grup, int czesc) {
    int* wskazniki = malloc((liczba_grup + 1) * sizeof(int)); /*Alokacja pamięci*/
    wskazniki[0] = 0; /*Pierwszy wskaźnik zawsze 0*/

	/*Przejście przez wszystkie grupy i zliczanie pasujących węzłów*/
    for (int g = 0; g < liczba_grup; g++) {
        int start = graf->grupy_ptr[g];
        int end = (g == liczba_grup - 1) ? elementy_grup : graf->grupy_ptr[g + 1];
        int licznik = 0;

        for (int i = start; i < end; i++) {
            if (przypisania[graf->grupy[i]] == czesc) {
                licznik++;
            }
        }
        wskazniki[g + 1] = wskazniki[g] + licznik; /*Kumulacja liczby węzłów*/
    }

    return wskazniki; /*Zwrócenie wskaźników*/
}

/*
Funkcja: zapisz_do_pliku_binarnego
Opis:
	Zapisuje podzielony graf do pliku binarnego w ustalonym formacie.
	Zapis obejmuje dane grafu oraz wskaźniki grup dla każdej partycji osobno.
Parametry:
	nazwa_pliku — nazwa pliku binarnego (ścieżka do pliku),
	graf — wskaźnik na strukturę grafu w formacie CSR,
	przypisania — tablica przypisująca węzły do części,
	liczba_wezlow — liczba wszystkich węzłów grafu,
	liczba_czesci — liczba części podziału grafu,
	liczba_grup — liczba grup w grafie,
	elementy_grup — łączna liczba elementów w tablicy grup.
Zwraca:
	Brak zwracanej wartości — zapisuje dane bezpośrednio do pliku binarnego.
*/
void zapisz_do_pliku_binarnego(const char* nazwa_pliku, MacierzCSR* graf, int* przypisania, int liczba_wezlow, int liczba_czesci, int liczba_grup, int elementy_grup) {
    FILE* plik = fopen(nazwa_pliku, "wb"); /*Otwarcie pliku binarnego*/
    if (!plik) {
        printf("Nie można utworzyć pliku binarnego: %s\n", nazwa_pliku);
        return;
    }

    /*1. Nagłówek: podstawowe informacje o grafie*/
    int naglowek[4] = {
        graf->max_w_wierszu,
        liczba_wezlow,
        liczba_grup,
        liczba_czesci
    };
    fwrite(naglowek, sizeof(int), 4, plik);

    /*2. Zapisanie podstawowych danych grafu*/
    fwrite(graf->indeksy, sizeof(int), liczba_wezlow, plik);
    fwrite(graf->indeksy_ptr, sizeof(int), graf->indeksy_ptr[liczba_wezlow] + 1, plik); // Uwaga: liczba_wierszy = graf->indeksy_ptr[liczba_wezlow] - 1
    fwrite(graf->grupy, sizeof(int), elementy_grup, plik);
    fwrite(graf->grupy_ptr, sizeof(int), liczba_grup + 1, plik);

    /*3. Zapisanie wskaźników grup dla każdej partycji*/
    for (int czesc = 0; czesc < liczba_czesci; czesc++) {
        // Oblicz wskaźniki grup dla partycji (jak wcześniej)
        int* wskazniki = oblicz_wskazniki_dla_partycji(graf, przypisania, liczba_wezlow, liczba_grup, elementy_grup, czesc);
        fwrite(wskazniki, sizeof(int), liczba_grup + 1, plik); // +1 dla zabezpieczenia
        free(wskazniki); /*Zwolnienie pamięci pomocniczej*/
    }

    fclose(plik); /*Zamknięcie pliku*/
    printf("Zapisano podział do pliku binarnego: %s\n", nazwa_pliku);
}

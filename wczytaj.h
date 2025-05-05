#ifndef WCZYTAJ_H
#define WCZYTAJ_H

#include "struktury.h"

#define MAX_LINIA 1000000	/*Maksymalny rozmiar linii*/

int wczytaj_wartosci(FILE* plik, int* tablica);
void oblicz_stopnie(MacierzCSR* graf, int liczba_wezlow, int liczba_grup, int liczba_elementow_grup);

#endif
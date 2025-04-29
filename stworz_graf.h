#ifndef STWORZ_GRAF_H
#define STWORZ_GRAF_H

#include "struktury.h"

MacierzLaplaceCSR* zbuduj_macierz_Laplace(MacierzCSR* graf, int liczba_wezlow);
MacierzLaplaceCSR* stworz_podgraf_Laplace(MacierzLaplaceCSR* L, int* indeksy, int liczba_wezlow);

#endif
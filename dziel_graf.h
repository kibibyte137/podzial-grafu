#ifndef DZIEL_GRAF_H
#define DZIEL_GRAF_H

#include "struktury.h"

void rekurencyjny_podzial(MacierzLaplaceCSR* L, int* indeksy, int liczba_wezlow, int* przypisania, int start_czesci, int liczba_czesci, int margines);
void oblicz_edges_inside_outside(MacierzLaplaceCSR* L, int* przypisania, int wezel, int* inside, int* outside);
void refine_balancing(MacierzLaplaceCSR* L, int* przypisania, int liczba_wezlow, int liczba_czesci, int margines);

#endif
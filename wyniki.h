#ifndef WYNIKI_H
#define WYNIKI_H

#include "struktury.h"

void zapisz_do_pliku_csrrg(const char* nazwa_pliku, MacierzCSR* graf, int* przypisania, int liczba_wezlow, int liczba_czesci, int liczba_grup, int elementy_grup, int liczba_wierszy);
int* oblicz_wskazniki_dla_partycji(MacierzCSR* graf, int* przypisania, int liczba_wezlow, int liczba_grup, int elementy_grup, int czesc);
void zapisz_do_pliku_binarnego(const char* nazwa_pliku, MacierzCSR* graf, int* przypisania, int liczba_wezlow, int liczba_czesci, int liczba_grup, int elementy_grup);

#endif
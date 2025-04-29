#ifndef OPERACJE_MACIERZOWE_H
#define OPERACJE_MACIERZOWE_H

#include "struktury.h"

int compare_double(const void* a, const void* b);
void mnoz_Lx(MacierzLaplaceCSR* L, double* x, double* wynik);
void metoda_potegowa(MacierzLaplaceCSR* L, double* fiedler, int iteracje);

#endif
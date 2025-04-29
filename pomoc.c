#include <stdio.h>
#include "pomoc.h"

void wyswietl_pomoc(void){
	printf("Podreczna pomoc\n");
	printf("\n");
	printf("Skompiluj program komenda: make\n");
	printf("Uruchom program komenda: ./podzial_grafu [opcje]\n");
	printf("\n");
	printf("Dostepne opcje:\n");
	printf("\t-f plik\t\t\tPlik wejsciowy z grafem (musi istniec i miec poprawny format).\n");
	printf("\t-o csrrg|bin\t\tFormat pliku wyjsciowego (csrrg - .csrrg [domyslny], bin - binarny).\n");
	printf("\t-c liczba_czesci\tLiczba czesci podzialu (min 2, max liczba wierzcholkow; domyslnie 2).\n");
	printf("\t-m margines\t\tMargines procentowy (1-100; domyslnie 10).\n");
	printf("\t-h\t\t\tWyswietla te pomoc.\n");
	printf("\n");
	printf("Przyklad uzycia:\n");
	printf("  ./podzial_grafu -f graf.csrrg -o csrrg -c 3 -m 15\n");
	printf("\n");
	printf("Program podzieli graf z pliku 'graf.csrrg' na 3 czesci\n");
	printf("z marginesem 15%%, zapisujac wynik w formacie .csrrg.\n");
}

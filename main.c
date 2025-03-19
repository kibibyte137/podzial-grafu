#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int czy_istnieje(const char *plik) {
    FILE *plik = fopen(plik, "r");
    if(plik){
        fclose(plik);
        return 1; /*Plik istnieje*/
    }
    return 0; /*Plik nie istnieje*/
}

/*Funkcja main programu*/
int main(int argc, char *argv[]){
	int opt; /*Zmienna przechowujaca opcje getopt*/
	int czesci = 2; /*Zmienna przechowujaca liczbe czesci, na ktora ma zostac podzielony graf podana przez uzytkownika, domyslnie 2*/
	int margines = 10; /*Zmienna przechowujaca margines procentowy ilosci wezlow w poszczegolnych czesciach grafu, domyslnie 10*/
	char *format_wyjsciowy = "txt"; /*Zmienna przechowujaca format pliku wejsciowego podany przez uzytkownika, domyslnie txt*/
	char *plik = NULL; /*Zmienna przechowujaca nazwe pliku podana przez uzytkownika*/
	
	while((opt = getopt(argc, argv, "f:o:c:m:h")) != -1){
		switch (opt) {
			case 'f':
				plik = optarg; /*Pobranie nazwy pliku podanej po -f*/
				break;
			case 'o':
				if (optarg) {
					format_wyjsciowy = optarg; /*Ustawienie formatu wyjsciowego, jezeli zostal podany*/
				}
				break;
			case 'c':
				if (optarg) {
					czesci = atoi(optarg); /*Ustawienie liczby czesci, na ktore ma zostac podzielony graf, jezeli zostala podana*/
				}
				break;
			case 'm':
				if (optarg) {
					margines = atoi(optarg); /*Ustawienie marginesu procentowego ilosci wezlow w poszczegolnych czesciach grafu, jezeli zostal podany*/
				}
				break;
			case 'h':
				// pomoc(); /*Wywolanie funkcji wyswietlajacej podreczna pomoc*/
				printf("Wyswietlenie podrecznej pomocy.\n");
				return 0; /*Zakonczenie programu po wyswietleniu pomocy */
			default:
				printf("Podano nieprawidlowa opcje.\n"); /*Blad dla nieznanych opcji*/
				// pomoc(); /*Wyswietlenie pomocy*/
				printf("Wyswietlenie podrecznej pomocy.\n");
				return 7; /*Zakonczenie programu z bledem*/
		}
	}
	
	
	/*OBSLUGA BLEDOW*/
	
	if !czy_istnieje(plik){
		printf("Nie znaleziono pliku: %s.", plik);
		return 1;
	}
	
	if(format_wyjsciowy != "txt" && format_wyjsciowy != "bin"){
		printf("Podano nieprawidlowy format pliku wyjsciowego: %s.", format_wyjsciowy);
		return 2;
	}
	
	if(margines < 0 || margines > 100){
		printf("Podano niepoprawny margines procentowy: %s.", margines);
		return 3;
	}
	
	/*OBSLUGA BLEDOW PO ODCZYTANIU GRAFU*/
	
	/*Niepoprawny format pliku: Nie udalo sie odczytac pliku: plik. Kod powrotu programu 5*/
	
	/* V - liczba wierzchołków w grafie
	if(czesci < 0 || czesci > V){
		printf("Nie mozna podzielic grafu na podana liczbe czesci: %s.", czesci);
		return 3;
	}
	*/
	
	/*DEBUG*/
	printf("Podany plik to: %s.\n", plik ? plik : "BRAK");
	printf("Przyjety format wyjsciowy to: %s.\n", format_wyjsciowy);
	printf("Przyjeta liczba czesci to: %d.\n", czesci);
	printf("Przyjety margines procentowy to: %d.\n", margines); 
	return 0;
}
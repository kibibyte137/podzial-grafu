#ifndef STRUKTURY_H
#define STRUKTURY_H

#define MAX_LINIA 1000000	/*Maksymalny rozmiar linii*/

typedef struct{
	int max_w_wierszu;		/*Maksymalna liczba wezlow w wierszu grafu (nie musi wystapic wiersz z taka iloscia wezlow)*/
	
	int* indeksy;			/*Indeksy wezlow w poszczegolnych wierszach*/
	int* indeksy_ptr;		/*Wskazniki na pierwsze indeksy węzłów w wierszach*/
	
	int* grupy;				/*Grupy wezlow polaczone przy pomocy krawedzi*/
	int* grupy_ptr;			/*Wskazniki na pierwsze wezly w poszczegolnych grupach*/
	
	int* stopnie;			/*Stopnie poszczegolnych wezlow*/
} MacierzCSR;

typedef struct{
	int n;					/*Liczba wezlow*/
	int nnz;				/*Liczba niezerowych elementów w macierzy Laplace'a*/
	int* row_ptr;			/*Wskazniki na pierwszy element wiersza*/
	int* col_idx;			/*Indeksy kolumn (sąsiedzi wierzcholka)*/
	double* values;			/*Wartosci macierzy Laplace'a L*/
} MacierzLaplaceCSR;

typedef struct{
	int* czesci;			/*Tablica przypisująca każdemu wierzchołkowi numer części, do której należy*/
	int liczba_czesci;		/*Liczba części, na które został podzielony graf*/
} PodzialGrafu;

#endif
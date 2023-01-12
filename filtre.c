// Copyright Sima Alexandru 312CA 2022-2023

#include "filtre.h"
#include "alocari.h"
#include "structuri.h"
#include "utilitare.h"
#include <stdio.h>
#include <string.h>

// Dimensiunea nucleului pentru filtrele aplicate
#define DIM_KER 3

// Filtrele disponibile.
enum filtre {
	EDGE,
	SHARPEN,
	BLUR,
	GAUSSIAN_BLUR,
	INVALID,
};

// Returneaza enumul corespunzatoru filtrului din `filtru`.
static enum filtre potrivire_filtru(const char *filtru);

// Verifica daca `poz` reprezinta coordonata unui pixel intr-o matrice cu
// dimensiunea `n` pentru care se aplica nucleul (pixelul se afla in selectia
// determinata de `st` si `dr`).
static int in_selectie(long poz, long st, long dr, long n);

// Verifica daca coordonatele `(i, j)` reprezinta un pixel pentru care
// se aplica nucleul (pixelul se afla in selectie si nu este pe margine).
static int pixel_in_selectie(long i, long j, struct imagine *img);

// Aplica efectul `nucleu` pe imaginea din `img`.
static int aplicare_nucleu(struct imagine *img,
						   const double nucleu[DIM_KER][DIM_KER]);

int aplicare_filtru(struct imagine *img, char *efect)
{
	if (!img) {
		puts("No image loaded");
		return 0;
	}
	if (!efect) {
		puts("Invalid command");
		return 0;
	}
	if (!img->color) {
		puts("Easy, Charlie Chaplin");
		return 0;
	}

	static const double efecte[][DIM_KER][DIM_KER] = {
		{
			// EDGE
			{-1, -1, -1},
			{-1, 8, -1},
			{-1, -1, -1},
		 },
		{
			// SHARPEN
			{0, -1, 0},
			{-1, 5, -1},
			{0, -1, 0},
		 },
		{
			// BLUR
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
		 },
		{
			// GAUSSIAN_BLUR
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
			{2.0 / 16, 4.0 / 16, 2.0 / 16},
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
		 },
	};

	int nr_filtru = potrivire_filtru(efect);
	if (nr_filtru == INVALID) {
		puts("APPLY parameter invalid");
		return 0;
	}
	if (aplicare_nucleu(img, efecte[nr_filtru]))
		return 1;

	printf("APPLY %s done\n", efect);
	return 0;
}

static enum filtre potrivire_filtru(const char *filtru)
{
	static const char *const comenzi[] = {
		"EDGE",
		"SHARPEN",
		"BLUR",
		"GAUSSIAN_BLUR",
	};

	for (enum filtre nr_filtru = EDGE; nr_filtru != INVALID; ++nr_filtru) {
		if (!strcmp(filtru, comenzi[nr_filtru]))
			return nr_filtru;
	}

	return INVALID;
}

static int in_selectie(long poz, long st, long dr, long n)
{
	return poz > 0 && poz >= st && poz < dr && poz < n - 1;
}

static int pixel_in_selectie(long i, long j, struct imagine *img)
{
	return in_selectie(i, img->st.i, img->dr.i, img->inaltime) &&
		   in_selectie(j, img->st.j, img->dr.j, img->latime);
}

static int aplicare_nucleu(struct imagine *img,
						   const double nucleu[DIM_KER][DIM_KER])
{
	union pixel **rez = aloca_matrice_pixeli(img->inaltime, img->latime);
	if (!rez)
		return 1;

	for (long i = 0; i < img->inaltime; ++i) {
		for (long j = 0; j < img->latime; ++j) {
			double suma_r = 0.0;
			double suma_g = 0.0;
			double suma_b = 0.0;

			// Pixelii care nu sunt selectati nu sunt afectati.
			if (!pixel_in_selectie(i, j, img)) {
				rez[i][j] = img->pixeli[i][j];
				continue;
			}

			for (int k = 0; k < 3; ++k) {
				for (int l = 0; l < 3; ++l) {
					// Pixelului (i, j) ii corespunde mijlocul nucleului (
					// (1, 1)), deci, cand k = 1,
					// indicele de linie este egal cu i
					// => i + k(=1) - k = i
					// => i + k - 1 este indicele de linie.
					// Analog pentru coloana.
					union pixel p = img->pixeli[i + k - 1][j + l - 1];
					suma_r += p.clr.rosu * nucleu[k][l];
					suma_g += p.clr.verde * nucleu[k][l];
					suma_b += p.clr.albastru * nucleu[k][l];
				}
			}

			// Restrange valorile in intervalul normal de culori.
			rez[i][j].clr = (struct culoare){
				restrange(suma_r, img->val_max),
				restrange(suma_g, img->val_max),
				restrange(suma_b, img->val_max),
			};
		}
	}

	eliberare_matrice_pixeli(img->pixeli, img->inaltime);
	img->pixeli = rez;
	return 0;
}

// Copyright Sima Alexandru 312CA (2022-2023)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "operatii.h"
#include "structuri.h"
#include "utilitare.h"

// Dimensiunea nucleului pentru filtrele aplicate
#define DIM_KER 3

// Sorteaza in ordine crescatoare valorile de la adresele `a` si `b`.
static void ordoneaza(long *a, long *b);

static void selectare_totala(struct imagine *img);
static void rotire_indici(struct coord src, struct coord *dest,
						  struct coord dim, int unghi);
static long *frecventa_pixeli(struct imagine *img, struct coord st,
							  struct coord dr, int nr_interv);

// Returneaza cea mai apropiata valoare de `x` din intervalul `[0; max]`.
static unsigned char restrange(double x, unsigned char max);

static int aplicare_nucleu(struct imagine *img,
						   const double nucleu[DIM_KER][DIM_KER]);

void selectare_suprafata(struct imagine *img, char *argumente)
{
	if (!img) {
		puts("No image loaded");
		return;
	}

	if (!argumente) {
		puts("Invalid command");
		return;
	}
	// Daca `argumente` contine stringul "ALL", se selecteaza toata imaginea.
	if (!strncmp(argumente, "ALL", strlen("ALL"))) {
		selectare_totala(img);
		puts("Selected ALL");
		return;
	}

	long x1, x2, y1, y2;
	if (!citire_numere(argumente, 4, &x1, &y1, &x2, &y2)) {
		puts("Invalid command");
		return;
	}

	// Coordonatele pot fi date in orice ordine, de aceea ele trebuie sortate,
	// pentru a se memora colturile din stanga-sus si dreapta-jos.
	ordoneaza(&x1, &x2);
	ordoneaza(&y1, &y2);

	// TODO
	if (x1 < 0 || x2 > img->latime || y1 < 0 || y2 > img->inaltime ||
		x1 == x2 || y1 == y2) {
		puts("Invalid set of coordinates");
		return;
	}

	img->st = (struct coord){y1, x1};
	img->dr = (struct coord){y2, x2};
	printf("Selected %ld %ld %ld %ld\n", x1, y1, x2, y2);
}

int rotire_imagine(struct imagine *img, char *argument)
{
	if (!img) {
		puts("No image loaded");
		return 0;
	}

	int unghi;

	if (!citire_numere(argument, 1, &unghi)) {
		puts("Unsupported rotation angle");
		return 0;
	}

	// Unghiul trebuie sa fie multiplu de 90 in intervalul [-360; 360].
	if (unghi > 360 || unghi < -360 || unghi % 90 != 0) {
		puts("Unsupported rotation angle");
		return 0;
	}

	long inaltime = img->dr.i - img->st.i;
	long latime = img->dr.j - img->st.j;
	long inaltime_rez = inaltime;
	long latime_rez = latime;

	if (inaltime == img->inaltime && latime == img->latime) {
		if (unghi % 180 != 0) {
			inaltime_rez = latime;
			latime_rez = inaltime;
		}
		// TODO mai frumos
	} else if (latime != inaltime && unghi % 180 != 0) {
		puts("The selection must be square");
		return 0;
	}

	union pixel **img_aux = aloca_matrice_pixeli(inaltime_rez, latime_rez);
	if (!img_aux) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	for (long i = 0; i < inaltime_rez; ++i) {
		for (long j = 0; j < latime_rez; ++j) {
			struct coord dest = {0, 0};
			rotire_indici((struct coord){i, j}, &dest,
						  (struct coord){inaltime_rez, latime_rez}, unghi);
			img_aux[i][j] = img->pixeli[img->st.i + dest.i][img->st.j + dest.j];
		}
	}

	if (inaltime == img->inaltime && latime == img->latime) {
		eliberare_matrice_pixeli(img->pixeli, inaltime);
		img->pixeli = img_aux;

		img->inaltime = inaltime_rez;
		img->latime = latime_rez;

		if (unghi % 180 != 0) {
			// TODO
			img->dr.i ^= img->dr.j;
			img->dr.j ^= img->dr.i;
			img->dr.i ^= img->dr.j;
		}
	} else {
		for (long i = 0; i < latime; ++i) {
			for (long j = 0; j < latime; ++j)
				img->pixeli[img->st.i + i][img->st.j + j] = img_aux[i][j];
		}
		eliberare_matrice_pixeli(img_aux, inaltime_rez);
	}

	printf("Rotated %d\n", unghi);
	return 0;
}

int egalizare_imagine(struct imagine *img)
{
	if (!img) {
		puts("No image loaded");
		return 0;
	}
	if (img->color) {
		puts("Black and white image needed");
		return 0;
	}

	long *frecv = frecventa_pixeli(img, (struct coord){0, 0},
								   (struct coord){img->inaltime, img->latime},
								   img->val_max + 1);
	if (!frecv) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	for (long i = 1; i <= img->val_max; ++i)
		frecv[i] += frecv[i - 1];

	long suprafata = img->inaltime * img->latime;
	for (long i = 0; i < img->inaltime; ++i) {
		for (long j = 0; j < img->latime; ++j) {
			unsigned char *p = &img->pixeli[i][j].val;
			*p = restrange((double)img->val_max * frecv[*p] / suprafata,
						   img->val_max);
		}
	}

	free(frecv);
	puts("Equalize done");
	return 0;
}

int decupare_imagine(struct imagine **img)
{
	if (!*img) {
		puts("No image loaded");
		return 0;
	}

	struct imagine *subimg = (struct imagine *)malloc(sizeof(struct imagine));
	if (!subimg) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	*subimg = **img;
	subimg->inaltime = (*img)->dr.i - (*img)->st.i;
	subimg->latime = (*img)->dr.j - (*img)->st.j;
	subimg->pixeli = aloca_matrice_pixeli(subimg->inaltime, subimg->latime);
	if (!subimg->pixeli) {
		fputs("malloc() failed", stderr);
		free(subimg);
		return 1;
	}

	for (long i = 0; i < subimg->inaltime; ++i) {
		for (long j = 0; j < subimg->latime; ++j)
			subimg->pixeli[i][j] =
				(*img)->pixeli[i + (*img)->st.i][j + (*img)->st.j];
	}
	selectare_totala(subimg);

	puts("Image cropped");
	eliberare_imagine(*img);
	*img = subimg;
	return 0;
}

int afisare_histograma(struct imagine *img, char *argumente)
{
	if (!img) {
		puts("No image loaded");
		return 0;
	}
	if (!argumente) {
		puts("Invalid command");
		return 0;
	}
	if (img->color) {
		puts("Black and white image needed");
		return 0;
	}
	long max_stelute, intervale;
	if (!citire_numere(argumente, 2, &max_stelute, &intervale)) {
		puts("Invalid command");
		return 0;
	}

	long *frecv_pixeli = frecventa_pixeli(img, img->st, img->dr, intervale);
	if (!frecv_pixeli) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	long frecv_max = 1;
	for (long i = 0; i < intervale; ++i) {
		if (frecv_pixeli[i] > frecv_max)
			frecv_max = frecv_pixeli[i];
	}

	for (int i = 0; i < intervale; ++i) {
		long nr_stelute = frecv_pixeli[i] * max_stelute / frecv_max;
		printf("%ld\t|\t", nr_stelute);
		for (long j = 0; j < nr_stelute; ++j)
			putchar('*');
		putchar('\n');
	}

	free(frecv_pixeli);
	return 0;
}

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

	int eroare = 0;
	// TODO
	if (!strcmp(efect, "EDGE")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{-1, -1, -1},
			{-1,	 8, -1},
			{-1, -1, -1},
		};
		eroare = aplicare_nucleu(img, nucleu);
	} else if (!strcmp(efect, "SHARPEN")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{ 0, -1,	0},
			{-1,	 5, -1},
			{ 0, -1,	0},
		};
		eroare = aplicare_nucleu(img, nucleu);
	} else if (!strcmp(efect, "BLUR")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
		};
		eroare = aplicare_nucleu(img, nucleu);
	} else if (!strcmp(efect, "GAUSSIAN_BLUR")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
			{2.0 / 16, 4.0 / 16, 2.0 / 16},
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
		};
		eroare = aplicare_nucleu(img, nucleu);
	} else {
		puts("APPLY parameter invalid");
		return 0;
	}

	if (eroare)
		return 1;
	printf("APPLY %s done\n", efect);
	return 0;
}

static void rotire_indici(struct coord src, struct coord *dest,
						  struct coord dim, int unghi)
{
	if (unghi < 0)
		unghi += 360;

	// (i, j) ajunge, dupa rotire_imagine, la:
	// (m - j, i)		- 90
	// (n - i, m - j)	- 180
	// (j, n - i)		- 270
	switch (unghi) {
	case 0:
	case 360:
		dest->i = src.i;
		dest->j = src.j;
		break;
	case 90:
		dest->i = dim.j - src.j - 1;
		dest->j = src.i;
		break;
	case 180:
		dest->i = dim.i - src.i - 1;
		dest->j = dim.j - src.j - 1;
		break;
	case 270:
		dest->i = src.j;
		dest->j = dim.i - src.i - 1;
		break;
	}
}

static void ordoneaza(long *a, long *b)
{
	if (*a > *b) {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
	}
}

static void selectare_totala(struct imagine *img)
{
	img->st = (struct coord){0, 0};
	img->dr = (struct coord){img->inaltime, img->latime};
}

static long *frecventa_pixeli(struct imagine *img, struct coord st,
							  struct coord dr, int nr_interv)
{
	long *frecv_pixeli = (long *)calloc(nr_interv, sizeof(long));
	if (!frecv_pixeli)
		return NULL;

	int nr_valori = (int)img->val_max + 1;
	long val_per_interv = nr_valori / nr_interv + (nr_valori % nr_interv != 0);
	for (long i = st.i; i < dr.i; ++i) {
		for (long j = st.j; j < dr.j; ++j)
			++frecv_pixeli[img->pixeli[i][j].val / val_per_interv];
	}

	return frecv_pixeli;
}

static unsigned char restrange(double x, unsigned char max)
{
	x = round(x);
	if (x < 0)
		return 0;
	if (x > max)
		return max;
	return (unsigned char)x;
}

static int in_selectie(long poz, long st, long dr, long n)
{
	return poz > 0 && poz < n - 1 && poz >= st && poz < dr;
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

			if (in_selectie(i, img->st.i, img->dr.i, img->inaltime) &&
				in_selectie(j, img->st.j, img->dr.j, img->latime)) {
				for (int k = 0; k < 3; ++k) {
					for (int l = 0; l < 3; ++l) {
						union pixel p = img->pixeli[i + k - 1][j + l - 1];
						suma_r += p.culoare.rosu * nucleu[k][l];
						suma_g += p.culoare.verde * nucleu[k][l];
						suma_b += p.culoare.albastru * nucleu[k][l];
					}
				}

				rez[i][j].culoare = (struct clr){
					restrange(suma_r, img->val_max),
					restrange(suma_g, img->val_max),
					restrange(suma_b, img->val_max),
				};
			} else {
				rez[i][j] = img->pixeli[i][j];
			}
		}
	}

	eliberare_matrice_pixeli(img->pixeli, img->inaltime);
	img->pixeli = rez;
	return 0;
}

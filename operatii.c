// Copyright Sima Alexandru 312CA (2022-2023)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "operatii.h"

// Dimensiunea nucleului pentru filtrele aplicate
#define DIM_KER 3

static void ordoneaza(long *a, long *b);
static void selectare_tot(struct imagine *img);
static long *frecventa_pixeli(struct imagine *img, struct coord st,
							  struct coord dr, int nr_interv);
static unsigned char restrange(double x, unsigned char max);

static int in_poza(struct imagine *img, long i, long j);
static int filtru(struct imagine *img, const double nucleu[DIM_KER][DIM_KER]);

void selectare_suprafata(struct imagine *img)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	long x1, x2, y1, y2;
	// Ia restul randului.
	char *selectie = strtok(NULL, "");

	if (!strcmp(selectie, "ALL")) {
		selectare_tot(img);
		printf("Selected 0 0 %ld %ld\n", img->latime, img->inaltime);
		return;
	}
	if (sscanf(selectie, " %ld %ld %ld %ld", &x1, &y1, &x2, &y2) != 4) {
		printf("Invalid coordinates\n");
		return;
	}

	ordoneaza(&x1, &x2);
	ordoneaza(&y1, &y2);

	if (x1 < 0 || x2 > img->latime || y1 < 0 || y1 > img->inaltime) {
		printf("Invalid coordinates\n");
		return;
	}

	img->st = (struct coord){y1, x1};
	img->dr = (struct coord){y2, x2};
	printf("Selected %ld %ld %ld %ld\n", x1, y1, x2, y2);
}

struct imagine *decupare_imagine(struct imagine *img)
{
	if (!img) {
		printf("No image loaded\n");
		return NULL;
	}

	struct imagine *subimg = (struct imagine *)malloc(sizeof(struct imagine));
	if (!subimg)
		return NULL;

	*subimg = *img;
	subimg->inaltime = img->dr.i - img->st.i;
	subimg->latime = img->dr.j - img->st.j;
	subimg->pixeli = aloca_matrice_pixeli(subimg->inaltime, subimg->latime);
	if (!subimg->pixeli) {
		free(subimg);
		return NULL;
	}

	for (long i = 0; i < subimg->inaltime; ++i) {
		for (long j = 0; j < subimg->latime; ++j)
			subimg->pixeli[i][j] = img->pixeli[i + img->st.i][j + img->st.j];
	}
	selectare_tot(subimg);

	printf("Image cropped\n");
	eliberare_imagine(img);
	return subimg;
}

void histograma(struct imagine *img)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}
	if (img->color) {
		printf("Black and white image needed\n");
		return;
	}

	char *argumente = strtok(NULL, "");
	long x, y;
	sscanf(argumente, " %ld %ld", &x, &y);

	long *frecv_pixeli = frecventa_pixeli(img, img->st, img->dr, x);
	if (!frecv_pixeli) {
		// TODO
		return;
	}

	long frecv_max = 0;
	for (long i = 0; i < x; ++i) {
		if (frecv_pixeli[i] > frecv_max)
			frecv_max = frecv_pixeli[i];
	}

	for (int i = 0; i < x; ++i) {
		long nr_stelute = frecv_pixeli[i] * y / frecv_max;
		printf("%ld\t|\t", nr_stelute);
		for (long i = 0; i < nr_stelute; ++i)
			putchar('*');
		putchar('\n');
	}

	free(frecv_pixeli);
}

void egalizare(struct imagine *img)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	long suprafata = img->inaltime * img->latime;

	long *frecv = frecventa_pixeli(img, (struct coord){0, 0},
								   (struct coord){img->inaltime, img->latime},
								   img->val_max + 1);
	if (!frecv) {
		// TODO
		return;
	}

	for (long i = 1; i <= img->val_max; ++i)
		frecv[i] += frecv[i - 1];

	for (long i = 0; i < img->inaltime; ++i) {
		for (long j = 0; j < img->latime; ++j) {
			unsigned char *p = &img->pixeli[i][j].val;
			*p = restrange((double)img->val_max * frecv[*p] / suprafata,
						   img->val_max);
		}
	}
	free(frecv);
}

void aplica(struct imagine *img)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	if (!img->color) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	char *efect = strtok(NULL, "");

	if (!strcmp(efect, "EDGE")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{-1, -1, -1},
			{-1, +8, -1},
			{-1, -1, -1},
		};
		filtru(img, nucleu);
	} else if (!strcmp(efect, "SHARPEN")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{ 0, -1,	0},
			{-1, +5, -1},
			{ 0, -1,	0},
		};
		filtru(img, nucleu);
	} else if (!strcmp(efect, "BLUR")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
		};
		filtru(img, nucleu);
	} else if (!strcmp(efect, "GAUSSIAN_BLUR")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
			{2.0 / 16, 4.0 / 16, 2.0 / 16},
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
		};
		filtru(img, nucleu);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}

	printf("APPLY %s done\n", efect);
}

static void ordoneaza(long *a, long *b)
{
	if (a > b) {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
	}
}

static void selectare_tot(struct imagine *img)
{
	img->st = (struct coord){0, 0};
	img->dr = (struct coord){img->inaltime, img->latime};
}

static long *frecventa_pixeli(struct imagine *img, struct coord st,
							  struct coord dr, int nr_interv)
{
	long *frecv_pixeli = (long *)calloc(nr_interv, sizeof(long));
	if (!frecv_pixeli) {
		// TODO
		return NULL;
	}

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

static int in_poza(struct imagine *img, long i, long j)
{
	return i > 0 && j > 0 && i < img->inaltime && j < img->latime;
}

static int filtru(struct imagine *img, const double nucleu[3][3])
{
	union pixel **rez = aloca_matrice_pixeli(img->inaltime, img->latime);
	if (!rez)
		return 1;

	for (long i = 0; i < img->inaltime; ++i) {
		for (long j = 0; j < img->latime; ++j) {
			double suma_r = 0.0;
			double suma_g = 0.0;
			double suma_b = 0.0;

			for (int k = 0; k < 3; ++k) {
				for (int l = 0; l < 3; ++l) {
					if (!in_poza(img, i + k - 1, j + l - 1))
						continue;

					union pixel p = img->pixeli[i + k - 1][j + l - 1];
					suma_r += p.culoare.r * nucleu[k][l];
					suma_g += p.culoare.g * nucleu[k][l];
					suma_b += p.culoare.b * nucleu[k][l];
				}
			}

			rez[i][j].culoare = (struct clr){
				restrange(suma_r, img->val_max),
				restrange(suma_g, img->val_max),
				restrange(suma_b, img->val_max),
			};
		}
	}

	free(img->pixeli);
	img->pixeli = rez;
	return 0;
}
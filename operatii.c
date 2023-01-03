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

	long x1, x2, y1, y2;

	if (!strcmp(argumente, "ALL")) {
		selectare_tot(img);
		printf("Selected 0 0 %ld %ld\n", img->latime, img->inaltime);
		return;
	}
	if (sscanf(argumente, " %ld %ld %ld %ld", &x1, &y1, &x2, &y2) != 4) {
		puts("Invalid coordinates");
		return;
	}

	ordoneaza(&x1, &x2);
	ordoneaza(&y1, &y2);

	if (x1 < 0 || x2 > img->latime || y1 < 0 || y1 > img->inaltime) {
		puts("Invalid coordinates");
		return;
	}

	img->st = (struct coord){y1, x1};
	img->dr = (struct coord){y2, x2};
	printf("Selected %ld %ld %ld %ld\n", x1, y1, x2, y2);
}

struct imagine *decupare_imagine(struct imagine *img)
{
	if (!img) {
		puts("No image loaded");
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

	puts("Image cropped");
	eliberare_imagine(img);
	return subimg;
}

void histograma(struct imagine *img, char *argumente)
{
	if (!img) {
		puts("No image loaded");
		return;
	}
	if (!argumente) {
		puts("Invalid command");
		return;
	}
	if (img->color) {
		puts("Black and white image needed");
		return;
	}

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
		puts("No image loaded");
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

void aplica(struct imagine *img, char *efect)
{
	if (!img) {
		puts("No image loaded");
		return;
	}
	if (!img->color) {
		puts("Easy, Charlie Chaplin");
		return;
	}
	if (!efect) {
		puts("Invalid command");
		return;
	}

	if (!strcmp(efect, "EDGE")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{-1, -1, -1},
			{-1, +8, -1},
			{-1, -1, -1},
		};
		aplicare_nucleu(img, nucleu);
	} else if (!strcmp(efect, "SHARPEN")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{ 0, -1,	0},
			{-1, +5, -1},
			{ 0, -1,	0},
		};
		aplicare_nucleu(img, nucleu);
	} else if (!strcmp(efect, "BLUR")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
		};
		aplicare_nucleu(img, nucleu);
	} else if (!strcmp(efect, "GAUSSIAN_BLUR")) {
		const double nucleu[DIM_KER][DIM_KER] = {
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
			{2.0 / 16, 4.0 / 16, 2.0 / 16},
			{1.0 / 16, 2.0 / 16, 1.0 / 16},
		};
		aplicare_nucleu(img, nucleu);
	} else {
		puts("APPLY parameter invalid");
		return;
	}

	printf("APPLY %s done\n", efect);
}

static void rotire_indici(struct coord src, struct coord *dest,
						  struct coord dim, int unghi)
{

	if (unghi < 0)
		unghi += 360;

	// (i, j) ajunge, dupa rotire, la:
	// (m - j, i)		- 90
	// (n - i, m - j) 	- 180
	// (j, n - i) 		- 270
	switch (unghi) {
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

void rotire(struct imagine *img, char *argument)
{
	if (!img) {
		puts("No image loaded!");
		return;
	}

	int unghi;
	if (sscanf(argument, "%d", &unghi) != 1) {
		puts("Unsupported rotation angle");
		return;
	}

	if (unghi == 0 || unghi > 360 || unghi < -360 || unghi % 90 != 0) {
		puts("Unsupported rotation angle");
		return;
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
	} else if (latime != inaltime) {
		puts("The selection must be square");
		return;
	}

	union pixel **img_aux = aloca_matrice_pixeli(inaltime_rez, latime_rez);
	if (!img_aux) {
		fputs("malloc() failed", stderr);
		eliberare_imagine(img);
		exit(EXIT_FAILURE);
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
	} else {
		for (long i = 0; i < latime; ++i) {
			for (long j = 0; j < latime; ++j)
				img->pixeli[img->st.i + i][img->st.j + j] = img_aux[i][j];
		}
		eliberare_matrice_pixeli(img_aux, inaltime_rez);
	}

	printf("Rotated %d\n", unghi);
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
			} else {
				rez[i][j] = img->pixeli[i][j];
			}
		}
	}

	eliberare_matrice_pixeli(img->pixeli, img->inaltime);
	img->pixeli = rez;
	return 0;
}

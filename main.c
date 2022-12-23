// Copyright Sima Alexandru 312CA (2022-2023)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "structuri.h"

static void ordoneaza(long *a, long *b)
{
	if (a > b) {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
	}
}

void selectare_tot(struct imagine *img)
{
	img->st = (struct coord){.x = 0, .y = 0};
	img->dr = (struct coord){
		.x = img->latime,
		.y = img->inaltime,
	};
}

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

	img->st = (struct coord){.x = x1, .y = y1};
	img->dr = (struct coord){.x = x2, .y = y2};
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
	subimg->latime = img->dr.x - img->st.x;
	subimg->inaltime = img->dr.y - img->st.y;
	subimg->pixeli = aloca_matrice_pixeli(subimg->inaltime, subimg->latime);
	if (!subimg->pixeli) {
		free(subimg);
		return NULL;
	}

	for (long i = 0; i < subimg->inaltime; ++i) {
		for (long j = 0; j < subimg->latime; ++j)
			subimg->pixeli[i][j] = img->pixeli[i + img->st.x][j + img->st.y];
	}
	selectare_tot(subimg);

	printf("Image cropped\n");
	eliberare_imagine(img);
	return subimg;
}

long *frecventa_pixeli(struct imagine *img, struct coord st, struct coord dr,
					   int nr_interv)
{
	long *frecv_pixeli = (long *)calloc(nr_interv, sizeof(long));
	if (!frecv_pixeli) {
		// TODO
		return NULL;
	}

	int nr_valori = (int)img->val_max + 1;
	long val_per_interv = nr_valori / nr_interv + (nr_valori % nr_interv != 0);
	for (long i = st.y; i < dr.y; ++i) {
		for (long j = st.x; j < dr.x; ++j)
			++frecv_pixeli[img->pixeli[i][j].val / val_per_interv];
	}

	return frecv_pixeli;
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

unsigned char restrange(double x, unsigned char max)
{
	x = round(x);
	if (x < 0)
		return 0;
	if (x > max)
		return max;
	return (unsigned char)x;
}

void egalizare(struct imagine *img)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	long suprafata = img->inaltime * img->latime;

	long *frecv = frecventa_pixeli(
		img, (struct coord){.x = 0, .y = 0},
		(struct coord){.x = img->latime, .y = img->inaltime}, img->val_max + 1);
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

void citire_comenzi()
{
	struct imagine *img = NULL;

	while (1) {
		char *linie = citire_linie(stdin);
		if (!linie) {
			// TODO
			eliberare_imagine(img);
			return;
		}

		char *comanda = strtok(linie, " ");
		if (!comanda) {
			printf("Invalid command\n");
			free(linie);
			continue;
		}

		if (!strcmp(comanda, "LOAD")) {
			if (img)
				eliberare_imagine(img);
			img = incarcare_fisier();
		} else if (!strcmp(comanda, "SELECT")) {
			selectare_suprafata(img);
		} else if (!strcmp(comanda, "HISTOGRAM")) {
			histograma(img);
		} else if (!strcmp(comanda, "EQUALIZE")) {
			egalizare(img);
			// TODO
		} else if (!strcmp(comanda, "ROTATE")) {
			// TODO
		} else if (!strcmp(comanda, "CROP")) {
			img = decupare_imagine(img);
		} else if (!strcmp(comanda, "APPLY")) {
			// TODO
		} else if (!strcmp(comanda, "SAVE")) {
			if (img)
				salvare_imagine(*img);
			else
				printf("No image loaded\n");
		} else if (!strcmp(comanda, "EXIT")) {
			if (img) {
				free(linie);
				eliberare_imagine(img);
				exit(EXIT_SUCCESS);
			}
			printf("No image loaded\n");
		} else {
			printf("Invalid command\n");
		}

		free(linie);
	}
}

int main(void)
{
	citire_comenzi();
	return 0;
}
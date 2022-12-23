// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "structuri.h"

void ordoneaza(long *a, long *b)
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

	long *frecv_pixeli = (long *)calloc(x, sizeof(long));
	if (!frecv_pixeli) {
		// TODO
		return;
	}

	long valori_per_interv = img->val_max / x + (img->val_max % x != 0);
	for (long i = 0; i < img->inaltime; ++i) {
		for (long j = 0; j < img->latime; ++j)
			++frecv_pixeli[img->pixeli[i][j].val / valori_per_interv];
	}

	long frecv_max = 0;
	for (long i = 0; i < x; ++i) {
		if (frecv_pixeli[i] > frecv_max)
			frecv_max = frecv_pixeli[i];
	}

	for (int i = 0; i < x; ++i) {
		long nr_stelute = frecv_pixeli[i] * x / frecv_max;
		printf("%ld\t|\t", nr_stelute);
		for (long i = 0; i < nr_stelute; ++i)
			putchar('*');
		putchar('\n');
	}

	free(frecv_pixeli);
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
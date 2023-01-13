// Copyright Sima Alexandru 312CA 2022-2023

#include <stdio.h>
#include <stdlib.h>

#include "histograme.h"
#include "utilitare.h"

// Functia calculeaza frecventa pixelilor din `img` cuprinsi intre `st` si `dr`,
// impartindu-i in `nr_interv` intervale de intensitate.
static long *frecventa_pixeli(struct imagine *img, struct coord st,
							  struct coord dr, int nr_interv);

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

	// Se calculeaza frecventa pixelilor pe toata
	// imaginea, indiferent de selectie.
	long *frecv = frecventa_pixeli(img, (struct coord){0, 0},
								   (struct coord){img->inaltime, img->latime},
								   img->val_max + 1);
	if (!frecv) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	// In frecv[i] retine frecventa intensitatilor <= i.
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

static long *frecventa_pixeli(struct imagine *img, struct coord st,
							  struct coord dr, int nr_interv)
{
	long *frecv_pixeli = (long *)calloc(nr_interv, sizeof(long));
	if (!frecv_pixeli)
		return NULL;

	int nr_valori = (int)img->val_max + 1;
	// Intr-un interval trebuie sa incapa
	// ceil(`nr_valori` / `nr_interv`) valori.
	long val_per_interv = nr_valori / nr_interv + (nr_valori % nr_interv != 0);
	for (long i = st.i; i < dr.i; ++i) {
		for (long j = st.j; j < dr.j; ++j)
			++frecv_pixeli[img->pixeli[i][j].val / val_per_interv];
	}

	return frecv_pixeli;
}

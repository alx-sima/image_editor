// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "operatii.h"
#include "utilitare.h"

// TODO
// Returneaza cifra din magic word a formatului.
static int verif_format(FILE *);

// Returneaza prima linie care nu contine un comentariu (nu incepe cu #).
static char *omite_comentarii(FILE *f);

// Aloca un vector in care citeste matricea de valori ascii cu
// `n` elemente din fisierul `f`.
static unsigned char *citeste_valori_ascii(FILE *f, long n);

// Aloca un vector in care citeste matricea de valori binare cu
// `n` elemente din fisierul `f`.
static unsigned char *citeste_valori_binar(FILE *f, long n);

char *citire_linie(FILE *fisier)
{
	char buffer[BUFSIZ];

	// Initial se aloca un string "gol" (care contine doar un '\0').
	char *linie = (char *)calloc(1, sizeof(char));
	size_t caract_stocate = 1;

	while (fgets(buffer, BUFSIZ, fisier)) {
		size_t caract_citite = strlen(buffer);
		caract_stocate += caract_citite;

		// Se concateneaza caracterele citite la rezultat.
		char *temp = (char *)realloc(linie, caract_stocate);
		if (!temp) {
			free(linie);
			return NULL;
		}
		linie = temp;
		// Se concateneaza caracterele citite in aceasta transa la rezultat.
		strncat(linie, buffer, caract_citite);

		// Daca ultimul caracter citit este newline,
		// s-a terminat de citit linia.
		if (linie[caract_stocate - 2] == '\n') {
			// Se elimina newline-ul.
			linie[caract_stocate - 2] = '\0';
			return (char *)realloc(linie, caract_stocate - 1);
		}
	}

	// Ultima linie poate sa nu contina '\n'
	if (feof(fisier))
		return linie;
	// In cazul in care nu s-a putut citi linia,
	// se elibereaza stringul si se intoarce NULL.
	free(linie);
	return NULL;
}

int incarcare_fisier(struct imagine **img, char *nume_fisier)
{
	if (*img)
		eliberare_imagine(*img);
	*img = NULL;

	FILE *fisier = fopen(nume_fisier, "r");
	if (!fisier) {
		printf("Failed to load %s\n", nume_fisier);
		return 0;
	}

	const int format = verif_format(fisier);
	// Imaginile in format ascii sunt P1...P3.
	const int ascii = format < 4;
	// Imaginile color sunt P3 si P6.
	const int color = format % 3 == 0;
	// Imaginile color folosesc cate 3 octeti
	// pt un pixel, iar restul cate 1.
	const int octeti_per_pixel = (color ? 3 : 1);

	char *linie = omite_comentarii(fisier);

	long latime, inaltime;
	int val_max = 1; // TODO
	citire_numere(linie, 2, &latime, &inaltime);

	// Imaginile alb-negru (P1 si P4) nu
	// au si o valoare maxima a culorii.
	if (format % 3 != 1) {
		free(linie);
		linie = omite_comentarii(fisier);
		val_max = strtol(linie, NULL, 10);
	}

	free(linie);
	unsigned char *a;
	if (ascii)
		a = citeste_valori_ascii(fisier, inaltime * latime * octeti_per_pixel);
	else
		a = citeste_valori_binar(fisier, inaltime * latime * octeti_per_pixel);
	if (!a) {
		fputs("malloc() fail", stderr);
		return 1;
	}

	*img = (struct imagine *)malloc(sizeof(struct imagine));
	(*img)->pixeli = aloca_matrice_pixeli(inaltime, latime);
	(*img)->val_max = val_max;
	(*img)->color = color;
	(*img)->inaltime = inaltime;
	(*img)->latime = latime;
	selectare_totala(*img);

	for (long i = 0; i < inaltime; ++i) {
		for (long j = 0; j < latime; ++j) {
			union pixel *p = &(*img)->pixeli[i][j];
			if (color) {
				p->clr.rosu =
					a[i * latime * octeti_per_pixel + j * octeti_per_pixel];
				p->clr.verde =
					a[i * latime * octeti_per_pixel + j * octeti_per_pixel + 1];
				p->clr.albastru =
					a[i * latime * octeti_per_pixel + j * octeti_per_pixel + 2];
			} else {
				p->val = a[i * latime + j];
			}
		}
	}

	printf("Loaded %s\n", nume_fisier);
	free(a);
	fclose(fisier);
	return 0;
}

void salvare_imagine(struct imagine *img, char *argumente)
{
	if (!img) {
		puts("No image loaded");
		return;
	}
	if (!argumente) {
		puts("Invalid command");
		return;
	}

	char *nume_fisier = strtok(argumente, " ");
	FILE *f = fopen(nume_fisier, "w");
	if (!f) {
		fprintf(stderr, "Cannot write to output file %s\n", nume_fisier);
		return;
	}

	int format = 1;
	if (img->color)
		format = 3;
	else if (img->val_max != 1)
		format = 2;

	char *ascii = strtok(NULL, " ");
	// Daca este prezent cuvantul ascii in comanda,
	// imaginea se salveaza in format text.
	if (!(ascii && !(strcmp(ascii, "ascii"))))
		format += 3;

	fprintf(f, "P%d\n", format);
	fprintf(f, "%lu %lu\n", img->latime, img->inaltime);
	// Imaginile alb-negru (P1 si P4) nu
	// au si o valoare maxima a culorii.
	if (format % 3 != 1)
		fprintf(f, "%d\n", img->val_max);

	for (long i = 0; i < img->inaltime; ++i) {
		for (long j = 0; j < img->latime; ++j) {
			union pixel p = img->pixeli[i][j];
			if (format < 4) {
				if (img->color)
					fprintf(f, "%hhu %hhu %hhu", p.clr.rosu, p.clr.verde,
							p.clr.albastru);
				else
					fprintf(f, "%hhu", p.val);

				if (j != img->latime - 1)
					fputc(' ', f);
			} else {
				if (img->color)
					fwrite(&p.clr, sizeof(unsigned char), 3, f);
				else
					fwrite(&p.val, sizeof(unsigned char), 1, f);
			}
		}
		if (format < 4)
			fputc('\n', f);
	}

	printf("Saved %s\n", nume_fisier);
	fclose(f);
}

// Returneaza cifra din magic word a formatului.
// TODO
static int verif_format(FILE *f)
{
	char format[4]; // TODO
	fgets(format, 4, f);
	return format[1] - '0';
}

static char *omite_comentarii(FILE *f)
{
	char *linie = citire_linie(f);
	while (linie[0] == '#') {
		free(linie);
		linie = citire_linie(f);
	}

	return linie;
}

static unsigned char *citeste_valori_ascii(FILE *f, long n)
{
	unsigned char *v = (unsigned char *)malloc(n * sizeof(unsigned char));
	if (!v)
		return NULL;

	char *linie = omite_comentarii(f);
	char *p = linie;
	char *q;

	long i = 0;
	while (i < n) {
		unsigned char val = (unsigned char)strtol(p, &q, 10);
		// Nu mai exista valori pe rand, se trece pe urmatorul.
		if (p == q) {
			free(linie);
			linie = citire_linie(f);
			p = linie;
			continue;
		}
		p = q;

		v[i++] = val;
	}
	free(linie);
	return v;
}

static unsigned char *citeste_valori_binar(FILE *f, long n)
{
	unsigned char *v = (unsigned char *)malloc(n * sizeof(unsigned char));
	if (!v)
		return NULL;

	fread(v, sizeof(unsigned char), n, f);
	return v;
}

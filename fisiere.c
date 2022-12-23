// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"

// TODO
// Returneaza cifra din magic word a formatului.
static int verif_format(FILE *f);
// Returneaza prima linie care nu contine un comentariu (nu incepe cu #).
static char *omite_comentarii(FILE *f);
static unsigned char *citeste_valori_ascii(FILE *f, long n);
static unsigned char *citeste_valori_binar(FILE *f, long n);

char *citire_linie(FILE *stream)
{
	char buffer[BUFSIZ];

	// Initial se aloca un string "gol" (care contine doar un '\0').
	char *rez = (char *)calloc(1, sizeof(char));
	size_t caract_stocate = 1;

	while (fgets(buffer, BUFSIZ, stream)) {
		size_t caract_citite = strlen(buffer);
		caract_stocate += caract_citite;

		char *rez_nou = (char *)realloc(rez, caract_stocate);
		if (!rez_nou) {
			free(rez);
			return NULL;
		}

		rez = rez_nou;
		strncat(rez, buffer, caract_citite);

		// Daca ultimul caracter citit este newline,
		// s-a terminat de citit linia.
		if (rez[caract_stocate - 2] == '\n') {
			// Se elimina newline-ul.
			rez[caract_stocate - 2] = '\0';
			return (char *)realloc(rez, caract_stocate - 1);
		}
	}

	if (feof(stream)) {
		return rez;
	}
	free(rez);
	return NULL;
}

struct imagine *incarcare_fisier()
{
	char *nume = strtok(NULL, "");

	FILE *f = fopen(nume, "r");
	if (!f) {
		printf("Failed to load %s\n", nume);
		return NULL;
	}

	int form = verif_format(f);
	int ascii = form < 4;
	int color = form % 3 == 0;
	// Imaginile color (P3 si P6) folosesc cate
	// 3 octeti pt un pixel, iar restul cate 1.

	int octeti_per_pixel = (color ? 3 : 1);

	char *linie = omite_comentarii(f);

	// TODO
	char *end;
	long latime = strtoll(linie, &end, 10);
	long inaltime = strtoll(end, NULL, 10);
	int val_max = 1; // TODO

	// Imaginile alb-negru nu au si o valoare maxima a culorii.
	if (form % 3 != 1) {
		free(linie);
		linie = omite_comentarii(f);
		val_max = strtol(linie, NULL, 10);
	}

	free(linie);
	unsigned char *a;
	if (ascii)
		a = citeste_valori_ascii(f, inaltime * latime * octeti_per_pixel);
	else
		a = citeste_valori_binar(f, inaltime * latime * octeti_per_pixel);
	if (!a) {
		// TODO
		return NULL;
	}

	struct imagine *img = (struct imagine *)malloc(sizeof(struct imagine));
	img->pixeli = aloca_matrice_pixeli(inaltime, latime);
	img->val_max = val_max;
	img->color = color;
	img->st = (struct coord){.x = 0, .y = 0};
	img->dr = (struct coord){.x = latime, .y = inaltime};
	img->inaltime = inaltime;
	img->latime = latime;

	for (long i = 0; i < inaltime; ++i) {
		for (long j = 0; j < latime; ++j) {
			union pixel *p = &img->pixeli[i][j];
			if (color) {
				p->culoare.r = a[i * latime + j * octeti_per_pixel];
				p->culoare.g = a[i * latime + j * octeti_per_pixel + 1];
				p->culoare.b = a[i * latime + j * octeti_per_pixel + 2];
			} else {
				p->val = a[i * latime + j];
			}
		}
	}
	printf("Loaded %s\n", nume);

	free(a);
	fclose(f);
	return img;
}

void salvare_imagine(struct imagine img)
{
	char *nume_fisier = strtok(NULL, " ");
	if (!nume_fisier) {
		// TODO
	}

	FILE *f = fopen(nume_fisier, "w");
	if (!f) {
		// TODO
	}

	int format = 1;
	if (img.color) {
		format = 3;
	} else if (img.val_max != 1) {
		format = 2;
	}

	char *ascii = strtok(NULL, " ");
	// Daca este prezent cuvantul ascii in comanda,
	// imaginea se salveaza in format text.
	if (!(ascii && !(strcmp(ascii, "ascii"))))
		format += 3;

	fprintf(f, "P%d\n", format);
	fprintf(f, "%lu %lu\n", img.latime, img.inaltime);
	if (format % 3 != 1)
		fprintf(f, "%d\n", img.val_max);

	for (long i = 0; i < img.inaltime; ++i) {
		for (long j = 0; j < img.latime; ++j) {
			union pixel p = img.pixeli[i][j];
			if (format < 4) {
				if (img.color)
					fprintf(f, "%d %d %d", p.culoare.r, p.culoare.g,
							p.culoare.b);
				else
					fprintf(f, "%d", p.val);

				if (j != img.latime - 1)
					fprintf(f, " ");
			} else {
				if (img.color)
					fwrite(&p.culoare, sizeof(unsigned char), 3, f);
				else
					fwrite(&p.val, sizeof(unsigned char), 1, f);
			}
		}
		if (format < 4)
			fprintf(f, "\n");
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
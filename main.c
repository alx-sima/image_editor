// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

union pixel {
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} culoare;
	unsigned char val;
};

struct imagine {
	union pixel **pixeli;
	unsigned char val_max;
	int n, m;
	int color;
};

// Citeste o linie intr-un string alocat dinamic.
// Intoarce NULL daca citirea nu s-a putut efectua.
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

// Returneaza prima linie care nu contine un comentariu (nu incepe cu #).
char *omite_comentarii(FILE *f)
{
	char *linie = citire_linie(f);
	while (linie[0] == '#') {
		free(linie);
		linie = citire_linie(f);
	}

	return linie;
}

unsigned char *citeste_valori_ascii(FILE *f, int n)
{
	unsigned char *v = (unsigned char *)malloc(n * sizeof(unsigned char));
	if (!v)
		return NULL;

	char *linie = omite_comentarii(f);
	char *p = linie;
	char *q;

	int i = 0;
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

unsigned char *citeste_valori_binar(FILE *f, int n)
{
	unsigned char *v = (unsigned char *)malloc(n * sizeof(unsigned char));
	if (!v)
		return NULL;

	fread(v, sizeof(unsigned char), n, f);

	return v;
}

// Returneaza cifra din magic word a formatului.
// TODO
int verif_format(FILE *f)
{
	char format[4]; // TODO
	fgets(format, 4, f);
	return format[1] - '0';
}

void eliberare_matrice_pixeli(union pixel **mat, int n)
{
	for (int i = 0; i < n; ++i)
		free(mat[i]);
	free(mat);
}

void eliberare_imagine(struct imagine *img)
{
	eliberare_matrice_pixeli(img->pixeli, img->n);
	free(img);
}

union pixel **aloca_matrice_pixeli(int n, int m)
{
	union pixel **mat = (union pixel **)malloc(n * sizeof(union pixel *));
	if (!mat)
		return NULL;

	for (int i = 0; i < n; ++i) {
		mat[i] = (union pixel *)malloc(m * sizeof(union pixel));
		if (!mat[i]) {
			eliberare_matrice_pixeli(mat, i);
			return NULL;
		}
	}

	return mat;
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
	int m = strtol(linie, &end, 10);
	int n = strtol(end, NULL, 10);
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
		a = citeste_valori_ascii(f, n * m * octeti_per_pixel);
	else
		a = citeste_valori_binar(f, n * m * octeti_per_pixel);
	if (!a) {
		// TODO
		return NULL;
	}

	struct imagine *img = (struct imagine *)malloc(sizeof(struct imagine));
	img->pixeli = aloca_matrice_pixeli(n, m);
	img->val_max = val_max;
	img->color = color;
	img->n = n;
	img->m = m;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			if (color) {
				img->pixeli[i][j].culoare.r = a[i * m + j * octeti_per_pixel];
				img->pixeli[i][j].culoare.g =
					a[i * m + j * octeti_per_pixel + 1];
				img->pixeli[i][j].culoare.b =
					a[i * m + j * octeti_per_pixel + 2];
			} else
				img->pixeli[i][j].val = a[i * m + j];
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
	fprintf(f, "%d %d\n", img.m, img.n);
	if (format % 3 != 1)
		fprintf(f, "%d\n", img.val_max);

	for (int i = 0; i < img.n; ++i) {
		for (int j = 0; j < img.m; ++j) {
			union pixel p = img.pixeli[i][j];
			if (format < 4) {
				if (img.color)
					fprintf(f, "%d %d %d", p.culoare.r, p.culoare.g,
							p.culoare.b);
				else
					fprintf(f, "%d", p.val);

				if (j != img.m - 1)
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

void citire_comanda(struct imagine **img)
{
	char *linie = citire_linie(stdin);
	if (!linie) {
		// TODO
		eliberare_imagine(*img);
		return;
	}

	char *comanda = strtok(linie, " ");
	if (!comanda) {
		printf("Invalid command\n");
		free(linie);
		return;
	}
	if (!strcmp(comanda, "LOAD")) {
		if (*img)
			eliberare_imagine(*img);
		*img = incarcare_fisier();
	} else if (!strcmp(comanda, "SELECT")) {
		// TODO
	} else if (!strcmp(comanda, "HISTOGRAM")) {
		// TODO
	} else if (!strcmp(comanda, "EQUALIZE")) {
		// TODO
	} else if (!strcmp(comanda, "ROTATE")) {
		// TODO
	} else if (!strcmp(comanda, "CROP")) {
		// TODO
	} else if (!strcmp(comanda, "APPLY")) {
		// TODO
	} else if (!strcmp(comanda, "SAVE")) {
		if (*img) {
			salvare_imagine(**img);
		} else {
			printf("No image loaded\n");
		}
	} else if (!strcmp(comanda, "EXIT")) {
		if (*img) {
			free(linie);
			eliberare_imagine(*img);
			exit(EXIT_SUCCESS);
		}
		printf("No image loaded\n");
	} else {
		printf("Invalid command\n");
	}

	free(linie);
}

int main(void)
{
	struct imagine *img = NULL;
	while (1) {
		citire_comanda(&img);
	}
	return 0;
}
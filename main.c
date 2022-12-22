// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int *citeste_valori(FILE *f, int n)
{
	int *v = (int *)malloc(n * sizeof(int));
	if (!v)
		return NULL;

	char *linie = omite_comentarii(f);
	char *p = linie;
	char *q;
	int i = 0;
	while (i < n) {
		int val = strtol(p, &q, 10);
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

// Returneaza cifra din magic word a formatului.
// TODO
int format(FILE *f)
{
	char format[4]; // TODO
	fgets(format, 4, f);
	return format[1] - '0';
}

void incarcare_fisier()
{
	char *nume = strtok(NULL, "");

	FILE *f = fopen(nume, "r");
	if (!f) {
		// TODO
		return;
	}

	int form = format(f);
	int ascii = form > 3;
	// Imaginile color (P3 si P6) folosesc cate
	// 3 octeti pt un pixel, iar restul cate 1.
	int octeti_per_pixel = (form % 3 == 0 ? 3 : 1);

	char *linie = omite_comentarii(f);

	// TODO
	char *end;
	int m = strtol(linie, &end, 10);
	int n = strtol(end, NULL, 10);

	if (form != 1) {
		free(linie);
		linie = omite_comentarii(f);
		int val_max = strtol(linie, NULL, 10);
	}

	free(linie);
	int *a = citeste_valori(f, n * m * octeti_per_pixel);
	if (!a) {
		// TODO
		return;
	}

	free(a);
	fclose(f);
}

void citire_comanda()
{
	char *linie = citire_linie(stdin);
	if (!linie) {
		// TODO
		return;
	}

	char *comanda = strtok(linie, " ");
	if (!comanda) {
		// TODO
		return;
	}
	if (!strcmp(comanda, "LOAD")) {
		incarcare_fisier();
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
		// TODO
	} else if (!strcmp(comanda, "EXIT")) {
		// TODO
	} else {
		printf("Invalid command\n");
	}

	free(linie);
}

int main(void)
{
	while (1) {
		citire_comanda();
	}
	return 0;
}
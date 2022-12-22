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

void citire_comanda()
{
	char *linie = citire_linie(stdin);
	if (!linie) {
		// TODO
	}

	char *comanda = strtok(linie, " ");
	if (!comanda) {
		// TODO
	}
	if (!strcmp(comanda, "LOAD")) {
		// TODO
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
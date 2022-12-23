// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "structuri.h"

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
// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "operatii.h"
#include "structuri.h"

char *sparge_rand(char *str)
{
	char *poz_spatiu = strchr(str, ' ');
	if (!poz_spatiu)
		return NULL;

	*poz_spatiu = '\0';
	return poz_spatiu + 1;
}

void citire_comenzi(void)
{
	struct imagine *img = NULL;

	while (1) {
		char *comanda = citire_linie(stdin);
		if (!comanda) {
			// TODO
			eliberare_imagine(img);
			return;
		}

		char *argumente = sparge_rand(comanda);

		if (!strcmp(comanda, "LOAD")) {
			if (img)
				eliberare_imagine(img);
			img = incarcare_fisier(argumente);
		} else if (!strcmp(comanda, "SELECT")) {
			selectare_suprafata(img, argumente);
		} else if (!strcmp(comanda, "HISTOGRAM")) {
			histograma(img, argumente);
		} else if (!strcmp(comanda, "EQUALIZE")) {
			egalizare(img);
		} else if (!strcmp(comanda, "ROTATE")) {
			rotire(img, argumente);
		} else if (!strcmp(comanda, "CROP")) {
			img = decupare_imagine(img);
		} else if (!strcmp(comanda, "APPLY")) {
			aplica(img, argumente);
		} else if (!strcmp(comanda, "SAVE")) {
			if (img)
				salvare_imagine(*img, argumente);
			else
				puts("No image loaded");
		} else if (!strcmp(comanda, "EXIT")) {
			if (img)
				eliberare_imagine(img);
			else
				puts("No image loaded");

			free(comanda);
			exit(EXIT_SUCCESS);
		} else {
			puts("Invalid command");
		}

		free(comanda);
	}
}

int main(void)
{
	citire_comenzi();
	return 0;
}

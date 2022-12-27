// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "operatii.h"
#include "structuri.h"

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
		} else if (!strcmp(comanda, "ROTATE")) {
			rotire(img);
		} else if (!strcmp(comanda, "CROP")) {
			img = decupare_imagine(img);
		} else if (!strcmp(comanda, "APPLY")) {
			aplica(img);
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
// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "operatii.h"
#include "structuri.h"
#include "utilitare.h"

enum comenzi {
	LOAD,
	SELECT,
	HISTOGRAM,
	EQUALIZE,
	ROTATE,
	CROP,
	APPLY,
	SAVE,
	EXIT,
	INVALID
};

static void citire_comenzi(void);

int main(void)
{
	citire_comenzi();
	return 0;
}

static void citire_comenzi(void)
{
	struct imagine *img = NULL;

	while (1) {
		char *comanda = citire_linie(stdin);
		if (!comanda) {
			eliberare_imagine(img);
			return;
		}
		char *argumente = sparge_comanda(comanda);

		static char *comenzi[] = {
			"LOAD", "SELECT", "HISTOGRAM", "EQUALIZE", "ROTATE",
			"CROP", "APPLY",  "SAVE",	   "EXIT",
		};

		enum comenzi nr_comanda;
		for (nr_comanda = LOAD; nr_comanda != INVALID; ++nr_comanda) {
			if (!strcmp(comanda, comenzi[nr_comanda]))
				break;
		}

		switch (nr_comanda) {
		case LOAD:
			if (img)
				eliberare_imagine(img);
			img = incarcare_fisier(argumente);
			break;
		case SELECT:
			selectare_suprafata(img, argumente);
			break;
		case HISTOGRAM:
			histograma(img, argumente);
			break;
		case EQUALIZE:
			egalizare(img);
			break;
		case ROTATE:
			rotire(img, argumente);
			break;
		case CROP:
			img = decupare_imagine(img);
			break;
		case APPLY:
			aplica(img, argumente);
			break;
		case SAVE:
			if (img)
				salvare_imagine(*img, argumente);
			else
				puts("No image loaded");
			break;
		case EXIT:
			if (img)
				eliberare_imagine(img);
			else
				puts("No image loaded");
			free(comanda);
			exit(EXIT_SUCCESS);
		default:
			puts("Invalid command");
		}

		free(comanda);
	}
}

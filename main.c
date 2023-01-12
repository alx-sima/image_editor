// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "filtre.h"
#include "fisiere.h"
#include "histograme.h"
#include "operatii.h"
#include "structuri.h"
#include "utilitare.h"

// Comenzile disponibile
enum comenzi {
	LOAD,
	SAVE,
	SELECT,
	CROP,
	ROTATE,
	APPLY,
	HISTOGRAM,
	EQUALIZE,
	EXIT,
	INVALID
};

// Returneaza enumul corespunzator comenzii din `comanda`.
static enum comenzi potrivire_comanda(const char *comanda);

// Bucla principala a programului in care se citesc si se executa comenzile.
static void citire_comenzi(void);

int main(void)
{
	citire_comenzi();
	return 0;
}

static enum comenzi potrivire_comanda(const char *comanda)
{
	static const char *const comenzi[] = {
		"LOAD",	 "SAVE",	  "SELECT",	  "CROP", "ROTATE",
		"APPLY", "HISTOGRAM", "EQUALIZE", "EXIT",
	};

	for (enum comenzi nr_comanda = LOAD; nr_comanda != INVALID; ++nr_comanda) {
		if (!strcmp(comanda, comenzi[nr_comanda]))
			return nr_comanda;
	}

	return INVALID;
}

static void citire_comenzi(void)
{
	struct imagine *img = NULL;

	while (1) {
		char *comanda = citire_linie(stdin);
		if (!comanda) {
			eliberare_imagine(img);
			fputs("malloc() fail", stderr);
			exit(EXIT_FAILURE);
		}
		char *argumente = sparge_comanda(comanda);

		enum comenzi nr_comanda = potrivire_comanda(comanda);
		int eroare = 0;
		switch (nr_comanda) {
		case LOAD:
			eroare = incarcare_fisier(&img, argumente);
			break;
		case SAVE:
			salvare_imagine(img, argumente);
			break;
		case SELECT:
			selectare_suprafata(img, argumente);
			break;
		case CROP:
			eroare = decupare_imagine(&img);
			break;
		case ROTATE:
			eroare = rotire_imagine(img, argumente);
			break;
		case APPLY:
			eroare = aplicare_filtru(img, argumente);
			break;
		case HISTOGRAM:
			eroare = afisare_histograma(img, argumente);
			break;
		case EQUALIZE:
			eroare = egalizare_imagine(img);
			break;
		case EXIT:
			if (!img)
				puts("No image loaded");
			free(comanda);
			eliberare_imagine(img);
			exit(EXIT_SUCCESS);
		default:
			puts("Invalid command");
		}

		free(comanda);
		if (eroare) {
			eliberare_imagine(img);
			exit(EXIT_FAILURE);
		}
	}
}

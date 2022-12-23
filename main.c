// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "fisiere.h"
#include "structuri.h"

void ordoneaza(long *a, long *b)
{
	if (a > b) {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
	}
}

void selectare_suprafata(struct imagine *img)
{
	long x1, x2, y1, y2;
	// Ia restul randului.
	char *selectie = strtok(NULL, "");

	if (!strcmp(selectie, "ALL")) {
		x1 = 0;
		y2 = 0;
		x2 = img->latime;
		y1 = img->inaltime;
	} else {
		if (sscanf(selectie, " %ld %ld %ld %ld", &x1, &y1, &x2, &y2) != 4) {
			printf("Invalid coordinates\n");
			return;
		}

		ordoneaza(&x1, &x2);
		ordoneaza(&y1, &y2);
	}
	printf("%ld %ld %ld %ld\n", x1, y1, x2, y2);
	if (x1 < 0 || y1 < 0 || x2 > img->latime || y1 > img->inaltime) {
		printf("Invalid coordinates\n");
		return;
	}

	img->st = (struct coord){.x = x1, .y = y1};
	img->dr = (struct coord){.x = x2, .y = y2};
}

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
			return;
		}

		if (!strcmp(comanda, "LOAD")) {
			if (img)
				eliberare_imagine(img);
			img = incarcare_fisier();
		} else if (!strcmp(comanda, "SELECT")) {
			if (img)
				selectare_suprafata(img);
			else
				printf("No image loaded\n");
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
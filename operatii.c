// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alocari.h"
#include "operatii.h"
#include "structuri.h"
#include "utilitare.h"

// Returneaza pozitia pixelului de la `(i, j)`
// dupa o rotire cu `unghi` grade spre dreapta.
static struct coord rotire_indici(long i, long j, long inaltime, long latime,
								  int unghi);

void selectare_suprafata(struct imagine *img, char *argumente)
{
	if (!img) {
		puts("No image loaded");
		return;
	}

	if (!argumente) {
		puts("Invalid command");
		return;
	}
	// Daca `argumente` contine stringul "ALL", se selecteaza toata imaginea.
	if (!strncmp(argumente, "ALL", strlen("ALL"))) {
		selectare_totala(img);
		puts("Selected ALL");
		return;
	}

	long x1, x2, y1, y2;
	if (!citire_numere(argumente, 4, &x1, &y1, &x2, &y2)) {
		puts("Invalid command");
		return;
	}

	// Coordonatele pot fi date in orice ordine, de aceea ele trebuie sortate,
	// pentru a se memora colturile din stanga-sus si dreapta-jos.
	ordoneaza(&x1, &x2);
	ordoneaza(&y1, &y2);

	// Coordonatele sunt valide daca sunt in interiorul
	// imaginii si suprafata selectata e nenula.
	if (x1 < 0 || x2 > img->latime || y1 < 0 || y2 > img->inaltime ||
		x1 == x2 || y1 == y2) {
		puts("Invalid set of coordinates");
		return;
	}

	// Se trece din coordonate de imagine (y, x) in (i, j).
	img->st = (struct coord){y1, x1};
	img->dr = (struct coord){y2, x2};
	printf("Selected %ld %ld %ld %ld\n", x1, y1, x2, y2);
}

void selectare_totala(struct imagine *img)
{
	img->st = (struct coord){0, 0};
	img->dr = (struct coord){img->inaltime, img->latime};
}

int rotire_imagine(struct imagine *img, char *argument)
{
	if (!img) {
		puts("No image loaded");
		return 0;
	}

	int unghi;

	if (!citire_numere(argument, 1, &unghi)) {
		puts("Unsupported rotation angle");
		return 0;
	}

	// Unghiul trebuie sa fie multiplu de 90 in intervalul [-360; 360].
	if (unghi > 360 || unghi < -360 || unghi % 90 != 0) {
		puts("Unsupported rotation angle");
		return 0;
	}

	long inaltime = img->dr.i - img->st.i;
	long latime = img->dr.j - img->st.j;

	// Dimensiunile rezultante ale imaginii.
	long inaltime_rez = inaltime;
	long latime_rez = latime;

	int sel_tot = inaltime == img->inaltime && latime == img->latime;

	// Daca rotatia se face cu un unghi care nu e multiplu de 180, dimensiunile
	// se inverseaza, deci trebuie ori ca toata imaginea sa fie selectata,
	// ori ca selectia sa fie patratica.
	if (unghi % 180 != 0) {
		if (sel_tot) {
			interschimba(&latime_rez, &inaltime_rez);
		} else if (latime != inaltime) {
			puts("The selection must be square");
			return 0;
		}
	}

	union pixel **img_aux = aloca_matrice_pixeli(inaltime_rez, latime_rez);
	if (!img_aux) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	for (long i = 0; i < inaltime_rez; ++i) {
		for (long j = 0; j < latime_rez; ++j) {
			struct coord dest =
				rotire_indici(i, j, inaltime_rez, latime_rez, unghi);
			img_aux[i][j] = img->pixeli[img->st.i + dest.i][img->st.j + dest.j];
		}
	}

	if (sel_tot) {
		eliberare_matrice_pixeli(img->pixeli, inaltime);

		img->pixeli = img_aux;
		img->inaltime = inaltime_rez;
		img->latime = latime_rez;

		if (unghi % 180 != 0)
			interschimba(&img->dr.i, &img->dr.j);
	} else {
		// Daca nu este rotita toata matricea, se coloreaza
		// pixel cu pixel selectia rotita.
		for (long i = 0; i < inaltime; ++i) {
			for (long j = 0; j < latime; ++j)
				img->pixeli[img->st.i + i][img->st.j + j] = img_aux[i][j];
		}
		eliberare_matrice_pixeli(img_aux, inaltime_rez);
	}

	printf("Rotated %d\n", unghi);
	return 0;
}

int decupare_imagine(struct imagine **img)
{
	if (!*img) {
		puts("No image loaded");
		return 0;
	}

	struct imagine *subimg = (struct imagine *)malloc(sizeof(struct imagine));
	if (!subimg) {
		fputs("malloc() failed", stderr);
		return 1;
	}

	// Subimaginea va pastra restul proprietatilor imaginii originale.
	*subimg = **img;
	subimg->inaltime = (*img)->dr.i - (*img)->st.i;
	subimg->latime = (*img)->dr.j - (*img)->st.j;

	subimg->pixeli = aloca_matrice_pixeli(subimg->inaltime, subimg->latime);
	if (!subimg->pixeli) {
		fputs("malloc() failed", stderr);
		free(subimg);
		return 1;
	}

	for (long i = 0; i < subimg->inaltime; ++i) {
		for (long j = 0; j < subimg->latime; ++j)
			subimg->pixeli[i][j] =
				(*img)->pixeli[i + (*img)->st.i][j + (*img)->st.j];
	}
	selectare_totala(subimg);

	puts("Image cropped");
	eliberare_imagine(*img);
	*img = subimg;
	return 0;
}

static struct coord rotire_indici(long i, long j, long inaltime, long latime,
								  int unghi)
{
	if (unghi < 0)
		unghi += 360;

	// (i, j) ajunge, dupa rotire, la:
	// (i, j)			- 0, 360
	// (m - j, i)		- 90
	// (n - i, m - j)	- 180
	// (j, n - i)		- 270
	switch (unghi) {
	case 90:
		return (struct coord){latime - j - 1, i};
	case 180:
		return (struct coord){inaltime - i - 1, latime - j - 1};
	case 270:
		return (struct coord){j, inaltime - i - 1};
	default:
		return (struct coord){i, j};
	}
}

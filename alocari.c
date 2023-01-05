// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdlib.h>

#include "alocari.h"

union pixel **aloca_matrice_pixeli(long n, long m)
{
	union pixel **mat = (union pixel **)malloc(n * sizeof(union pixel *));
	if (!mat)
		return NULL;

	for (long i = 0; i < n; ++i) {
		mat[i] = (union pixel *)malloc(m * sizeof(union pixel));
		if (!mat[i]) {
			eliberare_matrice_pixeli(mat, i);
			return NULL;
		}
	}

	return mat;
}

void eliberare_matrice_pixeli(union pixel **mat, long n)
{
	for (long i = 0; i < n; ++i)
		free(mat[i]);
	free(mat);
}

void eliberare_imagine(struct imagine *img)
{
	if (!img)
		return;
	eliberare_matrice_pixeli(img->pixeli, img->inaltime);
	free(img);
}

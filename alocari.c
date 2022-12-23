// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdlib.h>

#include "alocari.h"

// TODO
void eliberare_matrice_pixeli(union pixel **mat, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		free(mat[i]);
	free(mat);
}

void eliberare_imagine(struct imagine *img)
{
	eliberare_matrice_pixeli(img->pixeli, img->inaltime);
	free(img);
}

union pixel **aloca_matrice_pixeli(size_t n, size_t m)
{
	union pixel **mat = (union pixel **)malloc(n * sizeof(union pixel *));
	if (!mat)
		return NULL;

	for (size_t i = 0; i < n; ++i) {
		mat[i] = (union pixel *)malloc(m * sizeof(union pixel));
		if (!mat[i]) {
			eliberare_matrice_pixeli(mat, i);
			return NULL;
		}
	}

	return mat;
}
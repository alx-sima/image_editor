// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef ALOCARI_H
#define ALOCARI_H

#include "structuri.h"

// Aloca o matrice de `n` linii si `m` coloane de pixeli.
// Intoarce NULL daca intervine o eroare de alocare.
union pixel **aloca_matrice_pixeli(long n, long m);

// Elibereaza matricea de pixeli `mat` avand `n` linii`.
void eliberare_matrice_pixeli(union pixel **mat, long n);

// Elibereaza imaginea `img` (daca exista).
void eliberare_imagine(struct imagine *img);

#endif // ALOCARI_H

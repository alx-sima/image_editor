// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef _ALOCARI_H
#define _ALOCARI_H

#include "structuri.h"

void eliberare_imagine(struct imagine *img);

union pixel **aloca_matrice_pixeli(long n, long m);

#endif
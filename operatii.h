// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef _OPERATII_H
#define _OPERATII_H

#include "structuri.h"

void selectare_suprafata(struct imagine *img);

struct imagine *decupare_imagine(struct imagine *img);

void histograma(struct imagine *img);

void egalizare(struct imagine *img);

void aplica(struct imagine *img);

void rotire(struct imagine *img);

#endif // _OPERATII_H
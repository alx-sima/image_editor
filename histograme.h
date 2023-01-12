// Copyright Sima Alexandru 312CA 2022-2023

#ifndef HISTOGRAME_H
#define HISTOGRAME_H

#include "structuri.h"

// Calculeaza si afiseaza histograma imaginii `img`.
// Numarul de binuri si numarul maxim de stelute pentru
// reprezentare sunt transmise prin stringul `argumente`.
int afisare_histograma(struct imagine *img, char *argumente);

// Egalizeaza toata imaginea alb-negru `img`, aplatizandu-i histograma.
int egalizare_imagine(struct imagine *img);

#endif

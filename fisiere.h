// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef _FISIERE_H
#define _FISIERE_H

#include <stdio.h>

#include "structuri.h"

// Citeste o linie intr-un string alocat dinamic.
// Intoarce NULL daca citirea nu s-a putut efectua.
char *citire_linie(FILE *);

struct imagine *incarcare_fisier(char *nume_fisier);

void salvare_imagine(struct imagine img, char *argumente);

#endif // _FISIERE_H

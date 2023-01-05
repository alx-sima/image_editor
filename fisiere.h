// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef FISIERE_H
#define FISIERE_H

#include <stdio.h>

#include "structuri.h"

// Citeste o linie intr-un string alocat dinamic.
// Intoarce NULL daca citirea nu s-a putut efectua.
char *citire_linie(FILE *);

// Incarca imaginea din locatia `nume_fisier` in `img`.
int incarcare_fisier(struct imagine **img, char *nume_fisier);

// Salveaza imaginea `img`. Calea fisierului rezultat si modul de scriere
// (ascii/binar) sunt continute in stringul `argumente`.
void salvare_imagine(struct imagine img, char *argumente);

#endif // FISIERE_H

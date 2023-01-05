// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef OPERATII_H
#define OPERATII_H

#include "structuri.h"

// Selecteaza o subimagine din `img`, in functie de `argumente`:
// "ALL": se selecteaza toata imaginea.
// 2 perechi de coordonate: se selecteaza subimaginea determinata de cele 2
// perechi.
void selectare_suprafata(struct imagine *img, char *argumente);

// Inlocuieste imaginea `*img` cu subimaginea selectata.
int decupare_imagine(struct imagine **img);

// Roteste imaginea `img` cu unghiul din stringul `argument`.
// Unghiul poate fi doar multiplu de 90 si in intervalul [-360; 360].
// Daca se roteste o selectie a imaginii cu un unghi care nu e multiplu
// de 180, dimensiunile se inverseaza, asa ca selectia trebuie sa fie
// patratica.
int rotire_imagine(struct imagine *img, char *argument);

// Aplica filtrul `efect` pe selectia din `img`.
int aplicare_filtru(struct imagine *img, char *efect);

// Calculeaza si afiseaza histograma imaginii `img`.
// Numarul de binuri si numarul maxim de stelute pentru
// reprezentare sunt transmise prin stringul `argumente`.
int afisare_histograma(struct imagine *img, char *argumente);

// Egalizeaza toata imaginea alb-negru `img`.
int egalizare_imagine(struct imagine *img);

#endif // OPERATII_H

// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef OPERATII_H
#define OPERATII_H

#include "structuri.h"

// Selecteaza o subimagine din `img`, in functie de `argumente`:
// "ALL": se selecteaza toata imaginea.
// 2 perechi de coordonate: se selecteaza subimaginea determinata de cele 2
// perechi.
void selectare_suprafata(struct imagine *img, char *argumente);

// Selecteaza toata suprafata imaginii.
void selectare_totala(struct imagine *img);

// Inlocuieste imaginea `*img` cu subimaginea selectata.
int decupare_imagine(struct imagine **img);

// Roteste imaginea `img` cu unghiul din stringul `argument`.
// Unghiul poate fi doar multiplu de 90 si in intervalul [-360; 360].
// Daca se roteste o selectie a imaginii cu un unghi care nu e multiplu
// de 180, dimensiunile se inverseaza, asa ca selectia trebuie sa fie
// patratica.
int rotire_imagine(struct imagine *img, char *argument);

#endif // OPERATII_H

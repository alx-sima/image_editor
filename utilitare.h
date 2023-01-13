// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef UTILITARE_H
#define UTILITARE_H

#include <stdarg.h>

// Interschimba numerele de la adresele `a` si `b`.
void interschimba(long *a, long *b);

// Sorteaza in ordine crescatoare valorile de la adresele `a` si `b`.
void ordoneaza(long *a, long *b);

// Returneaza cea mai apropiata valoare de `x` din intervalul `[0; max]`.
unsigned char restrange(double x, unsigned char max);

// Modifica `s` astfel incat sa arate spre primul cuvant din
// string si intoarce un pointer spre restul stringului.
char *sparge_comanda(char *s);

// Citeste `nr` valori de tip long din stringul `s`
// si le stocheaza la adresele date.
// Returneaza 1 daca au fost citite **exact** `nr` valori.
int citire_numere(char *s, int nr, ...);

#endif // UTILITARE_H

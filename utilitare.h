// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef _UTILITARE_H
#define _UTILITARE_H

#include <stdarg.h>

// Modifica `str` astfel incat sa arate spre primul cuvant din
// string si intoarce un pointer spre restul stringului.
char *sparge_comanda(char *str);

// Citeste `nr` valori de tip long din stringul `str`
// si le stocheaza la adresele date.
// Returneaza 1 daca au fost citite **exact** `nr` valori.
int citire_numere(char *str, int nr, ...);

#endif // _UTILITARE_H

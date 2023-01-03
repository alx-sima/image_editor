// Copyright Sima Alexandru 312CA (2022-2023)

#include <stdlib.h>
#include <string.h>

#include "utilitare.h"

char *sparge_comanda(char *str)
{
	char *poz_spatiu = strchr(str, ' ');
	if (!poz_spatiu)
		return NULL;

	*poz_spatiu = '\0';
	return poz_spatiu + 1;
}

int citire_numere(char *str, int nr, ...)
{
	va_list vargs;
	va_start(vargs, nr);

	char *final = str;
	for (int i = 0; i < nr; ++i) {
		long *adr = va_arg(vargs, long *);

		*adr = strtol(str, &final, 10);
		if (final == str) {
			va_end(vargs);
			return 0;
		}
		str = final;
	}

	va_end(vargs);
	// Nu mai exista caractere de citit.
	return *final == '\0';
}

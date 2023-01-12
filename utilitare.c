// Copyright Sima Alexandru 312CA (2022-2023)

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "utilitare.h"

void interschimba(long *a, long *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

void ordoneaza(long *a, long *b)
{
	if (*a > *b)
		interschimba(a, b);
}

unsigned char restrange(double x, unsigned char max)
{
	x = round(x);
	if (x < 0)
		return 0;
	if (x > max)
		return max;
	return (unsigned char)x;
}

char *sparge_comanda(char *s)
{
	char *poz_spatiu = strchr(s, ' ');
	if (!poz_spatiu)
		return NULL;

	*poz_spatiu = '\0';
	return poz_spatiu + 1;
}

int citire_numere(char *s, int nr, ...)
{
	va_list argumente;
	va_start(argumente, nr);

	char *final = s;
	for (int i = 0; i < nr; ++i) {
		long *adr = va_arg(argumente, long *);

		*adr = strtol(s, &final, 10);
		if (final == s) {
			va_end(argumente);
			return 0;
		}
		s = final;
	}

	va_end(argumente);
	// Nu mai exista caractere de citit.
	return *final == '\0';
}

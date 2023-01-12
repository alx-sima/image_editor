// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef STRUCTURI_H
#define STRUCTURI_H

// Valorile RGB ale unui pixel.
struct culoare {
	unsigned char rosu;
	unsigned char verde;
	unsigned char albastru;
};

// Valorile pe care le poate lua
// un pixel (RGB sau alb-negru).
union pixel {
	struct culoare clr;
	unsigned char val;
};

// Coordonatele intr-o matrice.
struct coord {
	long i, j;
};

struct imagine {
	// Matricea de pixeli a imaginii.
	union pixel **pixeli;
	// Valoarea maxima a unui pixel.
	unsigned char val_max;
	// Colturile din stanga-sus si dreapta-jos ale selectiei.
	// Marginile din drepta si de jos nu sunt inclusa in selectie.
	struct coord st, dr;
	// Dimensiunile imaginii.
	long inaltime, latime;
	// Retine daca imaginea este color sau alb-negru.
	int color;
};

#endif // STRUCTURI_H

// Copyright Sima Alexandru 312CA (2022-2023)

#ifndef STRUCTURI_H
#define STRUCTURI_H

struct clr {
	unsigned char rosu;
	unsigned char verde;
	unsigned char albastru;
};

union pixel {
	struct clr culoare;
	unsigned char val;
};

struct coord {
	long i;
	long j;
};

struct imagine {
	union pixel **pixeli;
	unsigned char val_max;
	struct coord st;
	struct coord dr;
	long inaltime;
	long latime;
	int color;
};

#endif // STRUCTURI_H

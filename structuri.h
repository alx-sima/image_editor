#ifndef _STRUCTURI_H
#define _STRUCTURI_H

#include <stddef.h>

union pixel {
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} culoare;
	unsigned char val;
};

struct coord {
	int x;
	int y;
};

struct imagine {
	union pixel **pixeli;
	unsigned char val_max;
	struct coord st;
	struct coord dr;
	size_t inaltime;
	size_t latime;
	int color;
};

#endif // _STRUCTURI_H
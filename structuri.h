#ifndef _STRUCTURI_H
#define _STRUCTURI_H

struct clr {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

union pixel {
	struct clr culoare;
	unsigned char val;
};

struct coord {
	int i;
	int j;
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

#endif // _STRUCTURI_H

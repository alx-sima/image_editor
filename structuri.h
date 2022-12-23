#ifndef _STRUCTURI_H
#define _STRUCTURI_H

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
	long inaltime;
	long latime;
	int color;
};

#endif // _STRUCTURI_H
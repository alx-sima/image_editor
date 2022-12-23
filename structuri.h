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

struct imagine {
	union pixel **pixeli;
	unsigned char val_max;
	int n, m;
	int color;
};

#endif // _STRUCTURI_H
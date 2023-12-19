#include "processor.h"
#include "colour.h"

#include <stddef.h>

#define UNUSED __attribute__((__unused__))

static void UNUSED foreach(struct picture *pic, void (*func)(struct rgba *, void *), void *arg) {
	for (int i = 0; i < pic->height; i++) {
		for (int j = 0; j < pic->width; j++) {
			(*func)(&pic->data[i * pic->width + j], arg);
		}
	}
}

static void invert_lambda(struct rgba *cur, void *varg UNUSED) {
	cur->red = 255.0 - cur->red;
	cur->green = 255.0 - cur->green;
	cur->blue = 255.0 - cur->blue;
}

void invert(struct picture *pic) {
	foreach(pic, invert_lambda, NULL);
}

/*
void UNUSED_FUNC(grayscale)(struct picture *pic) {
}

void UNUSED_FUNC(rotate)(struct picture *pic, int angle) {
}

void UNUSED_FUNC(flip)(struct picture *pic, enum flip_direction dir) {
}
*/

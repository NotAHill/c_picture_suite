#include "processor.h"
#include "colour.h"

#define UNUSED_FUNC(x) __attribute__((__unused__)) UNUSED_ ## x

/*
static void UNUSED_FUNC(foreach)(struct picture *pic, void (*func)(void *)) {
}
*/

void invert(struct picture *pic) {
	if (pic->type == RGBA) {
		struct rgba *cur;
		for (int i = 0; i < pic->height; i++) {
			for (int j = 0; j < pic->width; j++) {
				cur = &pic->rgba[i * pic->width + j];

				cur->red = 255.0 - cur->red;
				cur->green = 255.0 - cur->green;
				cur->blue = 255.0 - cur->blue;
			}
		}
	} else {
		struct rgb *cur;
		for (int i = 0; i < pic->height; i++) {
			for (int j = 0; j < pic->width; j++) {
				cur = &pic->rgb[i * pic->width + j];

				cur->red = 255.0 - cur->red;
				cur->green = 255.0 - cur->green;
				cur->blue = 255.0 - cur->blue;
			}
		}
	}
}

/*
void UNUSED_FUNC(grayscale)(struct picture *pic) {
}

void UNUSED_FUNC(rotate)(struct picture *pic, int angle) {
}

void UNUSED_FUNC(flip)(struct picture *pic, enum flip_direction dir) {
}
*/

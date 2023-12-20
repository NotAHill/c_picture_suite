#include "processor.h"
#include "colour.h"

#include <stddef.h>
#include <stdlib.h>

#define UNUSED __attribute__((__unused__))
#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

static void foreach(struct picture *pic, void (*func)(struct rgba *, void *), void *arg) {
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

static void grayscale_lambda(struct rgba *cur, void *varg UNUSED) {
	int avg = (cur->red + cur->green + cur->blue) / 3;
	cur->red = cur->green = cur->blue = avg;
}

void invert(struct picture *pic) {
	foreach(pic, invert_lambda, NULL);
}

void grayscale(struct picture *pic) {
	foreach(pic, grayscale_lambda, NULL);
}

static void swap(struct rgba *a, struct rgba *b) {
	struct rgba tmp = *a;
	*a = *b;
	*b = tmp;
}

void flip(struct picture *pic, enum flip_direction dir) {
	if (dir == HORIZONTAL) {
		int half_width = pic->width / 2;
		for (int i = 0; i < pic->height; i++) {
			for (int j = 0; j < half_width; j++) {
				swap(&pic->data[i * pic->width + j], 
					 &pic->data[i * pic->width + pic->width - j - 1]);
			}
		}
	} else {
		int half_height = pic->height / 2;
		for (int i = 0; i < half_height; i++) {
			for (int j = 0; j < pic->width; j++) {
				swap(&pic->data[i * pic->width + j], &pic->data[(pic->height - i - 1) * pic->width + j]);
			}
		}
	}
}

void rotate(struct picture *pic, enum rotation_type rot) {
	if (rot == HALFTURN) {
		flip(pic, HORIZONTAL);
		flip(pic, VERTICAL);
	} else {
		// probably the least efficient way
		struct rgba *transpose = malloc(sizeof(struct rgba) * pic->width * pic->height);
		for (int i = 0; i < pic->height; i++) {
			for (int j = 0; j < pic->width; j++) {
				transpose[j * pic->height + i] = pic->data[i * pic->width + j];
			}
		}
		free(pic->data);
		pic->data = transpose;
		SWAP(pic->width, pic->height);
		if (rot == CLOCKWISE) {
			flip(pic, HORIZONTAL);
		} else {
			flip(pic, VERTICAL);
		}
	}
}

#include "processor.h"
#include "colour.h"

#include <stddef.h>
#include <stdlib.h>

#define UNUSED __attribute__((__unused__))
#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

static void foreach(struct picture *pic, void (*func)(struct picture *, int, int, void *), void *arg) {
	for (int i = 0; i < pic->height; i++) {
		for (int j = 0; j < pic->width; j++) {
			(*func)(pic, i, j, arg);
		}
	}
}

static void invert_lambda(struct picture *pic, int i, int j, void *varg UNUSED) {
	struct rgba *cur = &pic->data[i * pic->width + j];

	cur->red = 255.0 - cur->red;
	cur->green = 255.0 - cur->green;
	cur->blue = 255.0 - cur->blue;
}

static void grayscale_lambda(struct picture *pic, int i, int j, void *varg UNUSED) {
	struct rgba *cur = &pic->data[i * pic->width + j];

	int avg = (cur->red + cur->green + cur->blue) / 3;
	cur->red = cur->green = cur->blue = avg;
}

struct kernel_lambda_args {
	int *matrix;
	float scalar;
};

static void kernel_lambda(struct picture *pic, int i, int j, void *varg) {
	if (i == 0 || j == 0 || i == pic->height - 1 || j == pic->width - 1) return;

	struct kernel_lambda_args *kernel = (struct kernel_lambda_args *)varg;

	struct rgba cur = {0, 0, 0, 0};

	struct rgba *tmp;
	float scale_factor;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			tmp = &pic->data[(i + x) * pic->width + (j + y)];
			scale_factor = kernel->scalar * kernel->matrix[(x + 1) * 3 + (y + 1)];
			cur.red += (int)(tmp->red * scale_factor);
			cur.green += (int)(tmp->green * scale_factor);
			cur.blue +=(int)(tmp->blue * scale_factor);
		}
	}

	struct rgba *res = &pic->data[i * pic->width + j];
	res->red = cur.red;
	res->green = cur.green;
	res->blue = cur.blue;
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

void blur(struct picture *pic) {
	struct kernel_lambda_args kernel; 
	int kernel_matrix[] = {1, 1, 1,
					 	   1, 1, 1,
					 	   1, 1, 1};
	kernel.matrix = kernel_matrix;
	kernel.scalar = 1.0f / 9;
	foreach(pic, kernel_lambda, (void *)&kernel);
}

void identity(struct picture *pic) {
	struct kernel_lambda_args kernel; 
	int kernel_matrix[] = {0, 0, 0,
					 	   0, 1, 0,
					 	   0, 0, 0};
	kernel.matrix = kernel_matrix;
	kernel.scalar = 1.0f;
	foreach(pic, kernel_lambda, (void *)&kernel);
}


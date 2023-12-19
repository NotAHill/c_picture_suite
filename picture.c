#include "picture.h"
#include "colour.h"

#include <stdlib.h>
#include <stdio.h>

struct picture *create_picture(int width, int height) {
	struct picture *res = malloc(sizeof(struct picture));
	res->data = malloc(sizeof(struct rgb) * width * height);
	res->width = width;
	res->height = height;
	return res;
}

void free_picture(struct picture *pic) {
	free(pic->data);
	free(pic);
}

void print_picture(struct picture *pic) {
	for (int i = 0; i < pic->height; i++) {
		for (int j = 0; j < pic->width; j++) {
			print_rgb(&pic->data[i * pic->width + j]);
			printf(" ");
		}
		printf("\n");
	}
}

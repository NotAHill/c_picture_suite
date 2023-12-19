#include "picture.h"
#include "colour.h"

#include <stdlib.h>
#include <stdio.h>

struct picture *create_picture(int width, int height, int type) {
	struct picture *res = malloc(sizeof(struct picture));
	res->type = type;
	if (res->type == RGBA) res->rgba = malloc(sizeof(struct rgba) * width * height);
	else res->rgb = malloc(sizeof(struct rgb) * width * height);
	res->width = width;
	res->height = height;
	return res;
}

void free_picture(struct picture *pic) {
	if (pic->type == RGBA) free(pic->rgba);
	else free(pic->rgb);
	free(pic);
}

void print_picture(struct picture *pic) {
	for (int i = 0; i < pic->height; i++) {
		for (int j = 0; j < pic->width; j++) {
			if (pic->type == RGBA) print_rgba(&pic->rgba[i * pic->width + j]);
			else print_rgb(&pic->rgb[i * pic->width + j]);
			printf(" ");
		}
		printf("\n");
	}
}

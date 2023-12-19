#include "colour.h"

#include <stdio.h>

struct rgba to_rgba(uint32_t val) {
	static const uint32_t mask = 0x000000ff;
	struct rgba res;
	res.blue = mask & val;
	val >>= 8;
	res.green = mask & val;
	val >>= 8;
	res.red = mask & val;
	val >>= 8;
	res.alpha = mask & val;
	return res;
}

struct rgba to_rgb(uint32_t val) {
	static const uint32_t mask = 0x000000ff;
	struct rgba res;
	res.blue = mask & val;
	val >>= 8;
	res.green = mask & val;
	val >>= 8;
	res.red = mask & val;
	res.alpha = 0;
	return res;
}

uint32_t from_rgb(struct rgba *col) {
	return (uint32_t)0 + col->blue + (col->green << 8) + (col->red << 16);
}

uint32_t from_rgba(struct rgba *col) {
	return (uint32_t)0 + col->blue + (col->green << 8) + (col->red << 16) + (col->alpha << 24);
}

void print_rgb(struct rgba *col) {
	printf("(%u, %u, %u)", col->red, col->green, col->blue);
}

void print_rgba(struct rgba *col) {
	printf("(%u, %u, %u, %u)", col->red, col->green, col->blue, col->alpha);
}

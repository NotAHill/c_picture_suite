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

struct rgb to_rgb(uint32_t val) {
	static const uint32_t mask = 0x000000ff;
	struct rgb res;
	res.blue = mask & val;
	val >>= 8;
	res.green = mask & val;
	val >>= 8;
	res.red = mask & val;
	return res;
}

uint32_t from_rgb(struct rgb *col) {
	return (uint32_t)0 + col->red + (col->green << 8) + (col->blue << 16);
}

uint32_t from_rgba(struct rgba *col) {
	return (uint32_t)0 + col->alpha + (col->red << 8) + (col->green << 16) + (col->blue << 24);
}

void print_rgb(struct rgb *col) {
	printf("(%u, %u, %u)", col->red, col->green, col->blue);
}

void print_rgba(struct rgba *col) {
	printf("(%u, %u, %u, %u)", col->red, col->green, col->blue, col->alpha);
}

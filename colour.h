#ifndef COLOUR_H
#define COLOUR_H

#include <stdint.h>

struct rgba {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct rgb {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct rgba to_rgba(uint32_t);
struct rgb to_rgb(uint32_t);

uint32_t from_rgb(struct rgb *);
uint32_t from_rgba(struct rgba *);

void print_rgba(struct rgba *);
void print_rgb(struct rgb *);

#endif //COLOUR_H

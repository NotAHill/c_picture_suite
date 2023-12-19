#ifndef BMP_H
#define BMP_H

#include "picture.h"

#include <stdint.h>
#include <stdbool.h>

struct bmp {
	uint32_t pixel_array_offset;
	int32_t height;
	int32_t width;
	uint16_t bits_per_pixel;
	uint32_t *pixel_array;
	bool flipped;
};

struct picture *from_bmp(char *);

#endif //BMP_H

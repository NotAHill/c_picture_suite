#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "picture.h"

enum flip_direction {
	HORIZONTAL,
	VERTICAL
};

enum rotation_type {
	CLOCKWISE,
	HALFTURN,
	ANTICLOCKWISE
};

void invert(struct picture *);
void grayscale(struct picture *);
void rotate(struct picture *, enum rotation_type);
void flip(struct picture *, enum flip_direction);
void blur(struct picture *);
void identity(struct picture *);

#endif //PROCESSOR_H

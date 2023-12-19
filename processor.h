#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "picture.h"

enum flip_direction {
	HORIZONTAL,
	VERTICAL
};

void invert(struct picture *);
void grayscale(struct picture *);
void rotate(struct picture *, int);
void flip(struct picture *, enum flip_direction);

#endif //PROCESSOR_H

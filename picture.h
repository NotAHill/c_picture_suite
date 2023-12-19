#ifndef PICTURE_H
#define PICTURE_H

struct picture {
	enum {
		RGB = 24,
		RGBA = 32
	} type;
	union {
		struct rgba *rgba;
		struct rgb *rgb;
	};
	int width;
	int height;
};

struct picture *create_picture(int, int, int);
void free_picture(struct picture *);

void print_picture(struct picture *);

#endif //PICTURE_H

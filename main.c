#include "picture.h"
#include "bmp.h"
#include "colour.h"
#include "processor.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	struct picture *pic;
	if (argc == 1) {
		pic = from_bmp("./images/bmp/test.bmp");
	} else if (argc == 2) {
		pic = from_bmp(argv[1]);
	} else {
		fprintf(stderr, "Please enter only 1 command line argument\n");
		return 1;
	}
	print_picture(pic);

	if (argc == 1) {
		//pic->rgb[0] = (struct rgb) {128, 128, 128};
		invert(pic);
		to_bmp(pic, "./images/bmp/out.bmp");
	}

	free_picture(pic);
	return 0;
}

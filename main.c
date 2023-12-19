#include "picture.h"
#include "bmp.h"
#include "colour.h"
#include "processor.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	struct picture *pic;
	if (argc == 3) {
		pic = from_bmp(argv[1]);
	} else {
		fprintf(stderr, "Invalid command line args\n");
		return 1;
	}
	
	print_picture(pic);
	invert(pic);
	print_picture(pic);

	to_bmp(pic, argv[2]);

	free_picture(pic);

	return 0;
}

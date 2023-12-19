#include "picture.h"
#include "bmp.h"

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
	free_picture(pic);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#define BUFSIZE 1000

struct rgba {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

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

void print_rgba(struct rgba *col) {
	printf("(%u, %u, %u, %u)", col->red, col->green, col->blue, col->alpha);
}

struct bmp {
	uint32_t pixel_array_offset;
	int32_t height;
	int32_t width;
	uint16_t bits_per_pixel;
	uint32_t *pixel_array;
};


void process_bmp(char *path) {
	FILE *file;

	file = fopen(path, "r");
	if (file == NULL) {
		perror("Couldn't open file");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFSIZE];

	if (fread(buffer, 1, 2, file) != 2 || strncmp(buffer, "BM", 2) != 0) {
		fprintf(stderr, "Error: not a valid BMP file\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	printf("Found BMP signature\n");

	struct bmp img;

	fseek(file, 0xa, SEEK_SET);
	fread(&img.pixel_array_offset, 4, 1, file);

	printf("Offset at: %#" PRIx32 "\n", img.pixel_array_offset);

	fseek(file, 0x12, SEEK_SET);
	fread(&img.width, 4, 1, file);
	fread(&img.height, 4, 1, file);
	
	printf("Width: %d, Height: %d\n", img.width, img.height);

	fseek(file, 0x1c, SEEK_SET);
	fread(&img.bits_per_pixel, 2, 1, file);
	
	printf("Bits per pixel: %d\n", img.bits_per_pixel);

	fseek(file, img.pixel_array_offset, SEEK_SET);

	int img_size = img.width * abs(img.height);
	int bytes_per_pixel = img.bits_per_pixel / 8;


	if (bytes_per_pixel == 3) {
		img.pixel_array = calloc(img_size, sizeof(uint32_t));
		for (int i = 0; i < img_size; i += 2) {
			fread(img.pixel_array + i, bytes_per_pixel, 1, file);
			fread(img.pixel_array + i + 1, bytes_per_pixel, 1, file);
			fseek(file, 0x2, SEEK_CUR);
		}
	} else if (bytes_per_pixel == 4) {
		img.pixel_array = calloc(img_size, sizeof(uint32_t));
		fread(img.pixel_array, bytes_per_pixel, img_size, file);
	} else {
		fprintf(stderr, "Error: Current BBP not supported yet\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	if (img.height < 0) {
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				struct rgba col = to_rgba(img.pixel_array[i * img.width + j]);
				print_rgba(&col);
				printf(" ");
			}
			printf("\n");
		}
	} else {
		// picture is upside down
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				struct rgba col = to_rgba(img.pixel_array[(img.height - i - 1) * img.width + j]);
				print_rgba(&col);
				printf(" ");
			}
			printf("\n");
		}
	}

	fclose(file);
	free(img.pixel_array);
}

int main(void) {
	process_bmp("./images/test.bmp");
	return 0;
}

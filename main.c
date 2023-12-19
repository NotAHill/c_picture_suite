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

struct rgb {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
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

uint32_t from_rgb(struct rgb col) {
	return (uint32_t)0 + col.red + (col.green << 8) + (col.blue << 16);
}

struct bmp {
	uint32_t pixel_array_offset;
	int32_t height;
	int32_t width;
	uint16_t bits_per_pixel;
	uint32_t *pixel_array;
};

struct picture {
	struct rgb *data;
	int width;
	int height;
};

struct picture *create_picture(int width, int height) {
	struct picture *res = malloc(sizeof(struct picture));
	res->data = malloc(sizeof(struct rgb) * width * height);
	res->width = width;
	res->height = height;
	return res;
}

void free_picture(struct picture *pic) {
	free(pic->data);
	free(pic);
}


struct picture *from_bmp(char *path) {
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
	
	img.pixel_array = calloc(img_size, sizeof(uint32_t));

	if (bytes_per_pixel == 3) {
		for (int i = 0; i < img_size; i += 2) {
			fread(img.pixel_array + i, bytes_per_pixel, 1, file);
			fread(img.pixel_array + i + 1, bytes_per_pixel, 1, file);
			fseek(file, 0x2, SEEK_CUR);
		}
	} else if (bytes_per_pixel == 4) {
		fread(img.pixel_array, bytes_per_pixel, img_size, file);
	} else {
		fprintf(stderr, "Error: Current BBP not supported yet\n");
		fclose(file);
		free(img.pixel_array);
		exit(EXIT_FAILURE);
	}
	
	struct picture *res = create_picture(img.width, img.height);
	
	if (img.height < 0) {
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				res->data[i * img.width + j] = to_rgb(img.pixel_array[i * img.width + j]);
			}
		}
	} else {
		// picture is upside down
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				res->data[i * img.width + j] = to_rgb(img.pixel_array[i * img.width + j]);
			}
		}
	}
	
	fclose(file);
	free(img.pixel_array);

	return res;

}

void print_rgb(struct rgb *col) {
	printf("(%u, %u, %u)", col->red, col->green, col->blue);
}

void print_rgba(struct rgba *col) {
	printf("(%u, %u, %u, %u)", col->red, col->green, col->blue, col->alpha);
}

void print_picture(struct picture *pic) {
	for (int i = 0; i < pic->height; i++) {
		for (int j = 0; j < pic->width; j++) {
			print_rgb(&pic->data[i * pic->width + j]);
			printf(" ");
		}
		printf("\n");
	}
}


int main(void) {
	struct picture *pic = from_bmp("./images/bmp/test.bmp");
	print_picture(pic);
	free_picture(pic);
	return 0;
}

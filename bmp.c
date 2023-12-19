#include "bmp.h"
#include "colour.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#define BMP_ID_SIZE 2

#define BMP_PIXEL_ARRAY_OFFSET_LOC 0xa
#define BMP_PIXEL_ARRAY_OFFSET_SIZE 4

#define BMP_WIDTH_LOC 0x12
#define BMP_WIDTH_SIZE 4

#define BMP_HEIGHT_SIZE 4

#define BMP_BITS_PER_PIXEL_LOC 0x1c
#define BMP_BITS_PER_PIXEL_SIZE 2

static FILE *open_file(char *path) {
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		perror("Couldn't open file");
		exit(EXIT_FAILURE);
	}
	return file;
}

static void verify_bmp_signature(FILE *file) {
	char buffer[BMP_ID_SIZE + 100];

	if (fread(buffer, 1, BMP_ID_SIZE, file) != BMP_ID_SIZE || strncmp(buffer, "BM", BMP_ID_SIZE) != 0) {
		fprintf(stderr, "Error: not a valid BMP file\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	printf("Found BMP signature\n");
}



static void read_bmp_header(FILE *file, struct bmp *img) {
	fseek(file, BMP_PIXEL_ARRAY_OFFSET_LOC, SEEK_SET);
	fread(&img->pixel_array_offset, BMP_PIXEL_ARRAY_OFFSET_SIZE, 1, file);
	printf("Offset at: %#" PRIx32 "\n", img->pixel_array_offset);

	fseek(file, BMP_WIDTH_LOC, SEEK_SET);
	fread(&img->width, BMP_WIDTH_SIZE, 1, file);
	fread(&img->height, BMP_HEIGHT_SIZE, 1, file);
	
	if (img->height < 0) {
		img->flipped = false;
		img->height *= -1;
	} else {
		img->flipped = true;
	}

	printf("Width: %d, Height: %d\n", img->width, img->height);

	fseek(file, BMP_BITS_PER_PIXEL_LOC, SEEK_SET);
	fread(&img->bits_per_pixel, BMP_BITS_PER_PIXEL_SIZE, 1, file);
	
	printf("Bits per pixel: %d\n", img->bits_per_pixel);

	fseek(file, img->pixel_array_offset, SEEK_SET);
}

static void read_bmp_pixel_array(FILE *file, struct bmp *img) {
	int img_size = img->width * img->height;
	int bytes_per_pixel = img->bits_per_pixel / 8;
	
	img->pixel_array = calloc(img_size, sizeof(uint32_t));
	
	if (bytes_per_pixel == 3) {
		for (int i = 0; i < img_size; i += 2) {
			fread(img->pixel_array + i, bytes_per_pixel, 1, file);
			fread(img->pixel_array + i + 1, bytes_per_pixel, 1, file);
			fseek(file, 0x2, SEEK_CUR);
		}
	} else if (bytes_per_pixel == 4) {
		fread(img->pixel_array, bytes_per_pixel, img_size, file);
	} else {
		fprintf(stderr, "Error: Current BBP not supported yet\n");
		fclose(file);
		free(img->pixel_array);
		exit(EXIT_FAILURE);
	}
}

static void read_bmp_data(FILE *file, struct bmp *img) {
	read_bmp_header(file, img);

	read_bmp_pixel_array(file, img);
	
	fclose(file);
}

static void populate_picture_array(struct picture *pic, struct bmp *img) {
	int index;
	uint32_t cur;

	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			index = (img->flipped) ? img->height - i - 1 : i;
			cur = img->pixel_array[index * img->width + j];
			if (pic->type == RGBA) pic->rgba[i * img->width + j] = to_rgba(cur);
			else pic->rgb[i * img->width + j] = to_rgb(cur);
		}
	}
	
	free(img->pixel_array);
}

struct picture *from_bmp(char *path) {
	FILE *file = open_file(path);

	verify_bmp_signature(file);

	struct bmp img;

	read_bmp_data(file, &img);
	
	struct picture *res = create_picture(img.width, img.height, img.bits_per_pixel);

	populate_picture_array(res, &img);

	return res;
}

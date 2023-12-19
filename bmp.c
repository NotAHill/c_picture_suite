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

static FILE *open_file(char *path, char *mode) {
	FILE *file = fopen(path, mode);
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
	FILE *file = open_file(path, "r");

	verify_bmp_signature(file);

	struct bmp img;

	read_bmp_data(file, &img);
	
	struct picture *res = create_picture(img.width, img.height, img.bits_per_pixel);

	populate_picture_array(res, &img);

	return res;
}

#define UNUSED(type) (type){0}
#define BMP_HEADER_SIZE 14
#define DIB_HEADER_SIZE 40

#define write_attr(val) fwrite(&val, sizeof(val), 1, file)

void to_bmp(struct picture *pic, char *path) {
	FILE *file = open_file(path, "w");

	// needs to have the following parts
	// bitmap file header
	// 14 bytes

	struct {
		char id[BMP_ID_SIZE];
		uint32_t bmp_size;
		uint32_t pixel_array_offset;
	} bmp_header;

	struct {
		uint32_t dib_size;
		uint32_t bmp_width;
		uint32_t bmp_height;
		uint16_t color_plane_count;
		uint16_t bits_per_pixel;
		uint32_t pixel_compression;
		uint32_t pixel_data_size;
		uint32_t print_res_horizontal;
		uint32_t print_res_vertical;
		uint32_t palette_colour_count;
		uint32_t important_colour_count;
	} dib_header;

	bmp_header.id[0] = 'B';	bmp_header.id[1] = 'M';
	bmp_header.bmp_size = BMP_HEADER_SIZE + DIB_HEADER_SIZE + pic->width * pic->height;
	bmp_header.pixel_array_offset = BMP_HEADER_SIZE + DIB_HEADER_SIZE;

	dib_header.dib_size = DIB_HEADER_SIZE;
	dib_header.bmp_width = pic->width;
	dib_header.bmp_height = pic->height;
	dib_header.color_plane_count = 1;
	dib_header.bits_per_pixel = pic->type;
	dib_header.pixel_compression = 0;
	
	if (pic->type == RGBA)
		dib_header.pixel_data_size = pic->width * pic->height * (pic->type / 8);
	else {
		dib_header.pixel_data_size = pic->width * pic->height * (pic->type / 8 + 1);
	}

	dib_header.print_res_horizontal = 2835;
	dib_header.print_res_vertical = 2835;
	dib_header.palette_colour_count = 0;
	dib_header.important_colour_count = 0;

	write_attr(bmp_header.id);
	write_attr(bmp_header.bmp_size);
	write_attr(UNUSED(uint16_t));
	write_attr(UNUSED(uint16_t));
	write_attr(bmp_header.pixel_array_offset);

	write_attr(dib_header.dib_size);
	write_attr(dib_header.bmp_width);
	write_attr(dib_header.bmp_height);
	write_attr(dib_header.color_plane_count);
	write_attr(dib_header.bits_per_pixel);
	write_attr(dib_header.pixel_compression);
	write_attr(dib_header.pixel_data_size);
	write_attr(dib_header.print_res_horizontal);
	write_attr(dib_header.print_res_vertical);
	write_attr(dib_header.palette_colour_count);
	write_attr(dib_header.important_colour_count);

	int data_size = dib_header.bits_per_pixel / 8;
	int index;
	uint32_t raw;
	int counter = 0;

	for (int i = 0; i < pic->height; i++) {
		for (int j = 0; j < pic->width; j++) {
			index = (pic->height - i - 1) * pic->width + j;
			if (pic->type == RGBA) raw = from_rgba(&pic->rgba[index]);
			else raw = from_rgb(&pic->rgb[index]);
			
			fwrite(&raw, data_size, 1, file);

			if (pic->type == RGB && ++counter == 2) {
				write_attr(UNUSED(uint16_t));
				counter = 0;
			}
		}
	}

	fclose(file);


}

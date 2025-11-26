/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <io.h>
#include <system.h>

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;


static unsigned char gray_lut[65536];

unsigned char grayscale_cache[240 * 320];



/*
static unsigned char grayscale_array[MAX_W*MAX_H] __attribute__((aligned(32)));
static int grayscale_width = 0;
static int grayscape_height = 0;

*/

void conv_grayscale(void *picture,
		            int width,
		            int height) {
	int x,y,gray;
	unsigned short *pixels = (unsigned short *)picture , rgb;
	grayscale_width = width;
	grayscape_height = height;
	if (grayscale_array != NULL)
		free(grayscale_array);
	grayscale_array = (unsigned char *) malloc(width*height);
	for (y = 0 ; y < height ; y++) {
		for (x = 0 ; x < width ; x++) {
			rgb = pixels[y*width+x];
			gray = (((rgb>>11)&0x1F)<<3)*21; // red part
			gray += (((rgb>>5)&0x3F)<<2)*72; // green part
			gray += (((rgb>>0)&0x1F)<<3)*7; // blue part
			gray /= 100;
			IOWR_8DIRECT(grayscale_array,y*width+x,gray);
		}
	}
}

void init_gray_lut(void) {
	printf("Create LUT grayscale\n");
    for (int i = 0; i < 65536; i++) {
        int gray =
            (((i >> 11) & 0x1F) * 168) +
            (((i >> 5)  & 0x3F) * 288) +
            (((i >> 0)  & 0x1F) * 56);
        gray_lut[i] = gray / 100;
    }
    printf("Finished LUT grayscale\n");
}

void conv_grayscale_lut(void *picture, int width, int height)
{
    unsigned short *pixels = (unsigned short *)picture;

    grayscale_width  = width;
    grayscape_height = height;

    for (int y = 0; y < height; y++) {
        int row = y * width;
        for (int x = 0; x < width; x++) {
            int pos = row + x;
            grayscale_array[pos] = gray_lut[pixels[pos]];
        }
    }
}

int rgb565_swap(int v)
{
    return (v >> 8) | (v << 8);
}

int get_grayscale_width() {
	return grayscale_width;
}

int get_grayscale_height() {
	return grayscape_height;
}

unsigned char *get_grayscale_picture() {
	return grayscale_array;
}


void conv_grayscale_lut_block(void *picture,
                              unsigned char *gray,
                              int width,
                              int height,
                              int y_start,
                              int y_end)
{
    unsigned short *pixels = (unsigned short *)picture;

    // clamp (safety)
    if (y_start < 0)        y_start = 0;
    if (y_end   > height)   y_end   = height;

    for (int y = y_start; y < y_end; y++) {
        int row = y * width;
        for (int x = 0; x < width; x++) {
            int pos = row + x;
            grayscale_array[pos] = gray_lut[pixels[pos]];
        }
    }
}

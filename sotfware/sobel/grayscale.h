/*
 * grayscale.h
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_

extern unsigned char *grayscale_array;
extern int grayscale_width;
extern int grayscape_height;
extern unsigned char gray_lut[65536];

void conv_grayscale(void *picture,
		            int width,
		            int height);
int get_grayscale_width();
int get_grayscale_height();
unsigned char *get_grayscale_picture();
void conv_grayscale_lut();
void init_gray_lut();

#endif /* GRAYSCALE_H_ */

/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"

#define WIDTH  320
#define HEIGHT 240

const char gx_array[3][3] = {{-1,0,1},
                             {-2,0,2},
                             {-1,0,1}};
const char gy_array[3][3] = { {1, 2, 1},
                              {0, 0, 0},
                             {-1,-2,-1}};

short *sobel_x_result;
short *sobel_y_result;
unsigned short *sobel_rgb565;
unsigned char *sobel_result;
int sobel_width;
int sobel_height;

void init_sobel_arrays(int width , int height) {
	int loop;
	sobel_width = width;
	sobel_height = height;
	if (sobel_x_result != NULL)
		free(sobel_x_result);
	sobel_x_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_y_result != NULL)
		free(sobel_y_result);
	sobel_y_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)malloc(width*height*sizeof(unsigned char));
	if (sobel_rgb565 != NULL)
		free(sobel_rgb565);
	sobel_rgb565 = (unsigned short *)malloc(width*height*sizeof(unsigned short));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_x_result[loop] = 0;
		sobel_y_result[loop] = 0;
		sobel_result[loop] = 0;
		sobel_rgb565[loop] = 0;
	}
}

short sobel_mac( unsigned char *pixels,
                 int x,
                 int y,
                 const char *filter,
                 unsigned int width ) {
   short dy,dx;
   short result = 0;

   result += filter[0]* pixels[(y-1)*width+(x-1)]; // dy = -1, dx = -1
   result += filter[1]* pixels[(y-1)*width+(x)]; //dy = -1, dx = 0
   result += filter[2]* pixels[(y-1)*width+(x+1)]; //dy = -1, dx = 1

   result += filter[3]* pixels[y*width+(x-1)]; // dy = 0, dx = -1
   result += filter[4]* pixels[y*width+x]; // dy = 0, dx = 0
   result += filter[5]* pixels[y*width+(x+1)]; // dy = 0, dx = 1

   result += filter[6]* pixels[(y+1)*width+(x-1)]; // dy = 1, dx = -1
   result += filter[7]* pixels[(y+1)*width+(x)]; // dy = 1, dx = 0
   result += filter[8]*pixels[(y+1)*width+(x+1)]; // dy = 1, dx = 1

   /*
   for (dy = -1 ; dy < 2 ; dy++) {
      for (dx = -1 ; dx < 2 ; dx++) {
         result += filter[(dy+1)*3+(dx+1)]*
                   pixels[(y+dy)*width+(x+dx)];
      }
   }*/
   return result;
}

void sobel_x( unsigned char *source ) {
   int x,y;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
	    short result = 0;
		result += gx_array[0][0]* source[(y-1)*sobel_width+(x-1)]; // dy = -1, dx = -1
		result += gx_array[0][1]* source[(y-1)*sobel_width+(x)]; //dy = -1, dx = 0
		result += gx_array[0][2]* source[(y-1)*sobel_width+(x+1)]; //dy = -1, dx = 1

		result += gx_array[1][0]* source[y*sobel_width+(x-1)]; // dy = 0, dx = -1
		result += gx_array[1][1]* source[y*sobel_width+x]; // dy = 0, dx = 0
		result += gx_array[1][2]* source[y*sobel_width+(x+1)]; // dy = 0, dx = 1

		result += gx_array[2][0]* source[(y+1)*sobel_width+(x-1)]; // dy = 1, dx = -1
		result += gx_array[2][1]* source[(y+1)*sobel_width+(x)]; // dy = 1, dx = 0
		result += gx_array[2][2]*source[(y+1)*sobel_width+(x+1)]; // dy = 1, dx = 1

		sobel_x_result[y*sobel_width+x] = result;

         //sobel_x_result[y*sobel_width+x] = sobel_mac(source,x,y,gx_array,sobel_width);
      }
   }
}

void sobel_x_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gx_array,sobel_width);
          sobel_x_result[y*sobel_width+x] = result;
          if (result < 0) {
        	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
          } else {
        	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
          }
      }
   }
}

void sobel_y( unsigned char *source ) {
   int x,y;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	short result = 0;
    	result += gx_array[0][0]* source[(y-1)*sobel_width+(x-1)]; // dy = -1, dx = -1
		result += gx_array[0][1]* source[(y-1)*sobel_width+(x)]; //dy = -1, dx = 0
		result += gx_array[0][2]* source[(y-1)*sobel_width+(x+1)]; //dy = -1, dx = 1

		result += gx_array[1][0]* source[y*sobel_width+(x-1)]; // dy = 0, dx = -1
		result += gx_array[1][1]* source[y*sobel_width+x]; // dy = 0, dx = 0
		result += gx_array[1][2]* source[y*sobel_width+(x+1)]; // dy = 0, dx = 1

		result += gx_array[2][0]* source[(y+1)*sobel_width+(x-1)]; // dy = 1, dx = -1
		result += gx_array[2][1]* source[(y+1)*sobel_width+(x)]; // dy = 1, dx = 0
		result += gx_array[2][2]*source[(y+1)*sobel_width+(x+1)]; // dy = 1, dx = 1

	   sobel_y_result[y*sobel_width+x] = result;
         //sobel_y_result[y*sobel_width+x] = sobel_mac(source,x,y,gy_array,sobel_width);
      }
   }
}

void sobel_complete(unsigned char *source ){
	int x, y;
	int sum;

	for (y = 1; y < sobel_height - 1; y++) {

		int y_minus_1_width =(y-1)*sobel_width;
		int y_width = y*sobel_width;
		int y_add_1_width = (y+1)*sobel_width;

		for (x = 1; x < sobel_width - 1; x++) {

			int x_minus_1 = x-1;
			int x_add_1 = x+1;
			short gx = 0;
			short gy = 0;

			int source_y_min_x_min = source[y_minus_1_width + x_minus_1];
			int source_y_min_x_add = source[y_minus_1_width + x_add_1];
			int source_y_add_x_min = source[y_add_1_width + x_minus_1];
			int source_y_add_x_add = source[y_add_1_width + x_add_1];

			gx += -source_y_min_x_min +
					source_y_min_x_add -
					(source[y_width + x_minus_1]<<1) +
					(source[y_width + x_add_1]<<1) -
					source_y_add_x_min +
					source_y_add_x_add;

			gy += source_y_min_x_min +
					(source[y_minus_1_width + x]<<1) +
					source_y_min_x_add -
					source_y_add_x_min -
					(source[y_add_1_width + x]<<1) -
					source_y_add_x_add;

			//sobel_x_result[y_width + x] = gx;
			//sobel_y_result[y_width + x] = gy;

			// Threshold
			sum = (gx < 0) ? -gx : gx;
			sum += (gy < 0) ? -gy : gy;
			sobel_result[y_width + x] = (sum > 128) ? 0xFF : 0;

			/*
			printf("X opt, %d\n", gx);
			printf("Y opt, %d\n", gy);

			short gxx = 0;
			short gyy = 0;
			// Top row
			gxx += gx_array[0][0] * source[(y-1)*width + (x-1)];
			gxx += gx_array[0][1] * source[(y-1)*width + x];
			gxx += gx_array[0][2] * source[(y-1)*width + (x+1)];

			gyy += gy_array[0][0] * source[(y-1)*width + (x-1)];
			gyy += gy_array[0][1] * source[(y-1)*width + x];
			gyy += gy_array[0][2] * source[(y-1)*width + (x+1)];

			// Middle row
			gxx += gx_array[1][0] * source[y*width + (x-1)];
			gxx += gx_array[1][1] * source[y*width + x];
			gxx += gx_array[1][2] * source[y*width + (x+1)];

			gyy += gy_array[1][0] * source[y*width + (x-1)];
			gyy += gy_array[1][1] * source[y*width + x];
			gyy += gy_array[1][2] * source[y*width + (x+1)];

			// Bottom row
			gxx += gx_array[2][0] * source[(y+1)*width + (x-1)];
			gxx += gx_array[2][1] * source[(y+1)*width + x];
			gxx += gx_array[2][2] * source[(y+1)*width + (x+1)];

			gyy += gy_array[2][0] * source[(y+1)*width + (x-1)];
			gyy += gy_array[2][1] * source[(y+1)*width + x];
			gyy += gy_array[2][2] * source[(y+1)*width + (x+1)];


			printf("X good, %d\n", gxx);
			printf("Y good, %d\n", gyy);*/
		}
	}
}

void sobel_y_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gy_array,sobel_width);
         sobel_y_result[y*sobel_width+x] = result;
         if (result < 0) {
       	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
         } else {
       	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
         }
      }
   }
}

void sobel_threshold(short threshold) {
	int x,y,arrayindex;
	short sum,value;
	for (y = 1 ; y < (sobel_height-1) ; y++) {
		for (x = 1 ; x < (sobel_width-1) ; x++) {
			arrayindex = (y*sobel_width)+x;
			value = sobel_x_result[arrayindex];
			sum = (value < 0) ? -value : value;
			value = sobel_y_result[arrayindex];
			sum += (value < 0) ? -value : value;
			sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
		}
	}
}

unsigned short *GetSobel_rgb() {
	return sobel_rgb565;
}

unsigned char *GetSobelResult() {
	return sobel_result;
}

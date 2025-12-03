/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include "grayscale.h"
#include <system.h>

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

void sobel_grayscale_complete(void *picture)
{
    unsigned short *src = (unsigned short *)picture;
    const unsigned char *lut = gray_lut;
    unsigned char *dst = sobel_result;

    const int w = sobel_width;
    const int h = sobel_height;

    for (int y = 1; y < h - 1; y++)
    {
        int ym = (y - 1) * w;
        int yc =  y      * w;
        int yp = (y + 1) * w;

        unsigned short *row_m = src + ym;  // src pointers to each row
        unsigned short *row_c = src + yc;
        unsigned short *row_p = src + yp;

        unsigned char *dst_row = dst + yc; // output row

        for (int x = 1; x < w - 1; x++)
        {
            // Precompute neighbors for speed
            unsigned char p00 = lut[row_m[x-1]];
            unsigned char p01 = lut[row_m[x]];
            unsigned char p02 = lut[row_m[x+1]];

            unsigned char p10 = lut[row_c[x-1]];
            //unsigned char p11 = lut[row_c[x]];
            unsigned char p12 = lut[row_c[x+1]];

            unsigned char p20 = lut[row_p[x-1]];
            unsigned char p21 = lut[row_p[x]];
            unsigned char p22 = lut[row_p[x+1]];

            // Compute Gx and Gy EXACTLY as your original
            short gx = -p00 + p02
                       - ((short)p10 << 1)
                       + ((short)p12 << 1)
                       - p20 + p22;

            short gy =  p00 + ((short)p01 << 1) + p02
                       - p20 - ((short)p21 << 1) - p22;

            int sum = (gx < 0 ? -gx : gx) + (gy < 0 ? -gy : gy);

            dst_row[x] = (sum > 128) ? 0xFF : 0;
        }
    }
}

void sobel_grayscale_complete_fast(void *picture)
{
    unsigned short *src = (unsigned short *)picture;
    const unsigned char *lut = gray_lut;
    unsigned char *dst = sobel_result;

    const int w = sobel_width;
    const int h = sobel_height;

    for (int y = 1; y < h - 1; y++)
    {
        int ym = (y - 1) * w;
        int yc =  y      * w;
        int yp = (y + 1) * w;

        unsigned short *row_m = src + ym;  // src pointers to each row
        unsigned short *row_c = src + yc;
        unsigned short *row_p = src + yp;

        unsigned char *dst_row = dst + yc; // output row

        for (int x = 1; x < w - 1; x++)
        {
            // Precompute neighbors for speed
            unsigned char p00 = lut[row_m[x-1]];
            unsigned char p01 = lut[row_m[x]];
            unsigned char p02 = lut[row_m[x+1]];

            unsigned char p10 = lut[row_c[x-1]];
            //unsigned char p11 = lut[row_c[x]];
            unsigned char p12 = lut[row_c[x+1]];

            unsigned char p20 = lut[row_p[x-1]];
            unsigned char p21 = lut[row_p[x]];
            unsigned char p22 = lut[row_p[x+1]];


            unsigned int dataa =
                  (unsigned int)p00
                | ((unsigned int)p01 << 8)
                | ((unsigned int)p02 << 16)
                | ((unsigned int)p10 << 24);

            unsigned int datab =
                  (unsigned int)p12
                | ((unsigned int)p20 << 8)
                | ((unsigned int)p21 << 16)
                | ((unsigned int)p22 << 24);

            dst_row[x] = ALT_CI_SOBEL_0(dataa, datab);
        }
    }
}


void sobel_grayscale_complete_crop(void *picture)
{
    unsigned short *src = (unsigned short *)picture;
    const unsigned char *lut = gray_lut;
    unsigned char *dst = sobel_result;

    const int w = sobel_width;   // camera width 512
    const int h = sobel_height;  // camera height 384

    // ---- LCD window parameters ----
    const int lcd_offset = 16520;  // same as used in main
    const int lcd_w = 240;
    const int lcd_h = 320;

    // Convert offset
    int crop_y = lcd_offset / w;   // row
    int crop_x = lcd_offset % w;   // column

    // stay inside convolution borders
    int y_start = crop_y;
    int y_end   = crop_y + lcd_h - 1;
    int x_start = crop_x;
    int x_end   = crop_x + lcd_w - 1;

    if (y_start < 1)     y_start = 1;
    if (y_end   > h - 2) y_end   = h - 2;
    if (x_start < 1)     x_start = 1;
    if (x_end   > w - 2) x_end   = w - 2;

    for (int y = y_start; y <= y_end; y++)
    {
        int ym = (y - 1) * w;
        int yc =  y      * w;
        int yp = (y + 1) * w;

        unsigned short *row_m = src + ym;
        unsigned short *row_c = src + yc;
        unsigned short *row_p = src + yp;

        unsigned char *dst_row = dst + yc;

        for (int x = x_start; x <= x_end; x++)
        {

            unsigned char p00 = lut[row_m[x-1]];
            unsigned char p01 = lut[row_m[x]];
            unsigned char p02 = lut[row_m[x+1]];

            unsigned char p10 = lut[row_c[x-1]];
            // unsigned char p11 = lut[row_c[x]];
            unsigned char p12 = lut[row_c[x+1]];

            unsigned char p20 = lut[row_p[x-1]];
            unsigned char p21 = lut[row_p[x]];
            unsigned char p22 = lut[row_p[x+1]];

            unsigned int dataa =
                  (unsigned int)p00
                | ((unsigned int)p01 << 8)
                | ((unsigned int)p02 << 16)
                | ((unsigned int)p10 << 24);

            unsigned int datab =
                  (unsigned int)p12
                | ((unsigned int)p20 << 8)
                | ((unsigned int)p21 << 16)
                | ((unsigned int)p22 << 24);

            dst_row[x] = ALT_CI_SOBEL_0(dataa, datab);
        }
    }
}

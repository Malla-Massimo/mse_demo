#include <stdio.h>
#include <system.h>
#include <stdlib.h>
#include <io.h>
#include "lcd_simple.h"
#include "grayscale.h"
#include "i2c.h"
#include "camera.h"
#include "vga.h"
#include "dipswitch.h"
#include "sobel.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"

#define BLOCK_ROWS 32
extern unsigned char *grayscale_array;   // from grayscale.c
extern unsigned char *sobel_result;      // from sobel.c

void process_frame(void *picture, int width, int height);

int main() {
	printf("Hello from Nios II!\n");

	unsigned short *image;
	unsigned char *grayscale;
	unsigned char current_mode;
	unsigned char mode;
	init_LCD();
	init_camera();
	vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);

	cam_get_profiling();

	void *buffer1, *buffer2, *buffer3, *buffer4;
	buffer1 = (void *) malloc(cam_get_xsize() * cam_get_ysize());
	buffer2 = (void *) malloc(cam_get_xsize() * cam_get_ysize());
	buffer3 = (void *) malloc(cam_get_xsize() * cam_get_ysize());
	buffer4 = (void *) malloc(cam_get_xsize() * cam_get_ysize());


	cam_set_image_pointer(0, buffer1);
	cam_set_image_pointer(1, buffer2);
	cam_set_image_pointer(2, buffer3);
	cam_set_image_pointer(3, buffer4);
	enable_continues_mode();
	init_sobel_arrays(cam_get_xsize() >> 1, cam_get_ysize());

	init_gray_lut();

	do {
		printf("************************\n");
		if (new_image_available() != 0) {
			if (current_image_valid() != 0) {
				current_mode = DIPSW_get_value();
				mode = current_mode
						& (DIPSW_SW1_MASK | DIPSW_SW3_MASK | DIPSW_SW2_MASK);
				image = (unsigned short*) current_image_pointer();
				alt_timestamp_start();
				switch (mode) {
				case 0:
					transfer_LCD_with_dma(&image[16520], cam_get_xsize() >> 1,
							cam_get_ysize(), 0);
					if ((current_mode & DIPSW_SW8_MASK) != 0) {
						vga_set_swap(VGA_QuarterScreen);
						vga_set_pointer(image);
					}
					break;
				case 1:
					conv_grayscale((void *) image, cam_get_xsize() >> 1,
							cam_get_ysize());
					grayscale = get_grayscale_picture();
					transfer_LCD_with_dma(&grayscale[16520],
							cam_get_xsize() >> 1, cam_get_ysize(), 1);
					if ((current_mode & DIPSW_SW8_MASK) != 0) {
						vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);
						vga_set_pointer(grayscale);
					}
					break;
				case 2:
					conv_grayscale((void *) image, cam_get_xsize() >> 1,
							cam_get_ysize());
					grayscale = get_grayscale_picture();
					sobel_x_with_rgb(grayscale);
					image = GetSobel_rgb();
					transfer_LCD_with_dma(&image[16520], cam_get_xsize() >> 1,
							cam_get_ysize(), 0);
					if ((current_mode & DIPSW_SW8_MASK) != 0) {
						vga_set_swap(VGA_QuarterScreen);
						vga_set_pointer(image);
					}
					break;
				case 3:
					conv_grayscale((void *) image, cam_get_xsize() >> 1,
							cam_get_ysize());
					grayscale = get_grayscale_picture();
					alt_timestamp_start();
					alt_u32 start_x = alt_timestamp();
					sobel_x(grayscale);
					alt_u32 mid = alt_timestamp();
					sobel_y_with_rgb(grayscale);
					alt_u32 end_y = alt_timestamp();

					printf("case 3\n");
					alt_u32 sb_x = mid - start_x;
					printf("Sobel X: %u\n", sb_x);

					alt_u32 sb_y = end_y - mid;
					printf("Sobel Y: %u\n", sb_y);

					image = GetSobel_rgb();
					transfer_LCD_with_dma(&image[16520], cam_get_xsize() >> 1,
							cam_get_ysize(), 0);
					if ((current_mode & DIPSW_SW8_MASK) != 0) {
						vga_set_swap(VGA_QuarterScreen);
						vga_set_pointer(image);
					}
					break;
				default:
					printf("Start sobel");
					alt_u32 gray_start = alt_timestamp();
					//conv_grayscale((void *) image, cam_get_xsize() >> 1, cam_get_ysize());
					conv_grayscale_lut((void *) image, cam_get_xsize() >> 1, cam_get_ysize());
					grayscale = get_grayscale_picture();
					alt_u32 gray_end = alt_timestamp();
					alt_u32 gray_conv_time = gray_end - gray_start;
					printf("Gray conv : %u\n", gray_conv_time);

					alt_u32 mid_sobel = alt_timestamp();
					sobel_complete(grayscale);
					alt_u32 end_sobelY = alt_timestamp();
					grayscale = GetSobelResult();

					alt_u32 sb2_y = end_sobelY - mid_sobel;
					printf("Sobel complete : %u\n", sb2_y);


					/*
					printf("start cache");
					alt_u32 data_cache_start = alt_timestamp();
					process_frame((void *) image, cam_get_xsize() >> 1, cam_get_ysize());
					grayscale = GetSobelResult();
					alt_u32 data_cache_end = alt_timestamp();
					printf("data cache complete : %u\n", data_cache_end-data_cache_start);
					*/
					transfer_LCD_with_dma(&grayscale[16520],
							cam_get_xsize() >> 1, cam_get_ysize(), 1);


					if ((current_mode & DIPSW_SW8_MASK) != 0) {
						vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);
						vga_set_pointer(grayscale);
					}
					break;
				}
			}
		}
	} while (1);
	return 0;
}


void process_frame(void *picture, int width, int height)
{
    for (int y = 1; y < height - 1; y += BLOCK_ROWS) {

        int y_end = y + BLOCK_ROWS;
        if (y_end > height - 1)
            y_end = height - 1;

        // 1) Grayscale only for rows [y-1, y_end+1) because Sobel needs neighbors
        int gry_start = y - 1;
        int gry_end   = y_end + 1;

        if (gry_start < 0)         gry_start = 0;
        if (gry_end   > height)    gry_end   = height;

        conv_grayscale_lut_block(picture, grayscale_array, width, height,
                                 gry_start, gry_end);

        // 2) Sobel only for rows [y, y_end)
        sobel_complete_block(grayscale_array, sobel_result,
                             width, height, y, y_end);
    }
}

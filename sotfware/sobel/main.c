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


int main() {

	unsigned short *image;
	unsigned char *grayscale;
	unsigned char current_mode;
	unsigned char mode;
	init_LCD();
	printf("LCD INIT\n");
	init_camera();
	printf("CAMERA INIT\n");
	vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);
	printf("Hello from Nios II!\n");
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
		if (new_image_available() != 0) {
			if (current_image_valid() != 0) {
				printf("*\n");
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
					;
					// Grayscale + sobel

					alt_u32 gray_start = alt_timestamp();
					sobel_grayscale_complete_crop((void *) image);
					grayscale = GetSobelResult();
					alt_u32 gray_end = alt_timestamp();
					printf("Gray conv : %u\n", gray_end-gray_start);

					/*
					alt_u32 gray_start = alt_timestamp();
					conv_grayscale_lut((void *) image, cam_get_xsize() >> 1, cam_get_ysize());
					grayscale = get_grayscale_picture();
					sobel_complete(grayscale);
					grayscale = GetSobelResult();
					alt_u32 end_sobelY = alt_timestamp();
					printf("Sobel complete : %u\n", end_sobelY);
					/*
					/*
					alt_u32 mid_sobel = alt_timestamp();
					sobel_grayscale_complete((void *) image);
					alt_u32 end_sobelY = alt_timestamp();
					//sobel_threshold(128);
					grayscale = GetSobelResult();
					alt_u32 sb2_y = end_sobelY - mid_sobel;
					printf("Sobel complete : %u\n", sb2_y);
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "curve_graphic.h"

#define WIDTH 500
#define HEIGHT 500

int main(void)
{
	FILE* image_file = fopen("img.ppm", "wb");
	(void)fprintf(image_file, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
//	Edit the file here
	image_buffer* image_test = image_buffer_create(WIDTH, HEIGHT, 3);
//	plot_line(400, 195, 200, 200, 0xffffffff, image_test);	
//	memset(image_test->buffer, 0xff, image_buffer_size_b(image_test));
	plot_bezier(100, 300, 250, 200, 400, 300, 0xffffffff, 60, image_test);
	fwrite((void*)image_test->buffer, image_buffer_size_b(image_test), 
			1, image_file);
	image_buffer_destroy(image_test);
	(void)fclose(image_file);
	return 0x45;
}

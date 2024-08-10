#include <stdlib.h>
#include <stdint.h>

typedef struct
{
	uint8_t* buffer;
	uint32_t width, height;
	int n_channels;
}image_buffer;

void plot_line(int x0, int y0, int x1, int y1, uint32_t color, image_buffer* buffer);

void plot_lineAA(int x0, int y0, int x1, int y1, uint32_t color, image_buffer* buffer);

void plot_bezier(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, int resolution, 
		image_buffer* buffer);

image_buffer* image_buffer_create(uint32_t width, uint32_t height, int n_channels);

void image_buffer_destroy(image_buffer* p);

size_t image_buffer_size_b(image_buffer* p);

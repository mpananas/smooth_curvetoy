#include "curve_graphic.h"
#include <stdio.h>
#include <math.h>

static inline void clamp_edge_and_sort(int* x0, int* y0, int* x1, int* y1, image_buffer* buffer);
static inline float lerp(float p0, float p1, float t);
static inline void swap_ints(int* a, int* b);
static inline void write_image_buffer(int x, int y, uint32_t color, image_buffer* buffer);
static inline uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b);
static inline uint32_t scale_color(uint32_t color, float t);
static inline uint8_t get_color_component(uint32_t color, int component);

void plot_bezier(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, int resolution, 
		image_buffer* buffer)
{
	float t_step = 1.0f / (float)resolution;
	float t_val = t_step;
	int prev_x = x0;
	int prev_y = y0;
	for(int i = 0; i < resolution; i++)
	{
		float r1x = lerp((float)x0, (float)x1, t_val);
		float r1y = lerp((float)y0, (float)y1, t_val);

		float r2x = lerp((float)x1, (float)x2, t_val);
		float r2y = lerp((float)y1, (float)y2, t_val);

		int curr_x = (int)lerp(r1x, r2x, t_val);
		int curr_y = (int)lerp(r1y, r2y, t_val);
		printf("%d | %d | %d | %d\n", curr_x, prev_x, curr_y, prev_y);

		plot_line(curr_x, curr_y, prev_x, prev_y, color, buffer);

		prev_x = curr_x;
		prev_y = curr_y;

		t_val += t_step;
	}
}

void plot_line(int x0, int y0, int x1, int y1, uint32_t color, image_buffer* buffer)
{
	int dx =  abs(x1 - x0), x_step = (x0 < x1) ? 1 : -1;
	int dy = -abs(y1 - y0), y_step = (y0 < y1) ? 1 : -1;
	int err = dx+dy, e2;

	for(;;)
	{
		write_image_buffer(x0, y0, color, buffer);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 <= dx) { err += dx; y0 += y_step; }
		if (e2 >= dy) { err += dy; x0 += x_step; }
	}
}

void plot_lineAA(int x0, int y0, int x1, int y1, uint32_t color, image_buffer* buffer)
{
	int dx =  abs(x1 - x0), x_step = (x0 < x1) ? 1 : -1;
	int dy = -abs(y1 - y0), y_step = (y0 < y1) ? 1 : -1;
	int err = dx - dy, temp_err, temp_x;
	int e_dist = ((dx + dy) == 0) ? 1 : sqrt((float)(dx * dx) + (float)(dy * dy));

	for(;;)
	{
		int channel = 255 * abs(err - dx - dy) / e_dist;
		write_image_buffer(x0, y0, pack_color(channel, channel, channel), buffer);
		if (x0 == x1 && y0 == y1) break;
		temp_err = err;
		temp_x = x0;
		if(2 * temp_err >= -dx)
		{
			if(x0 == x1) { break; }
			if(temp_err < e_dist - dy)
			{
				int channel_y = 255 * (temp_err + dy) / e_dist;
				write_image_buffer(x0, y0 + y_step, pack_color(channel_y, channel_y, channel_y), buffer);
			}
			err -= dy;
			x0 += x_step;
		}
		if(2 * temp_err <= dy)
		{
			if(y0 == y1) { break; }
			if(dx < e_dist + temp_err)
			{
				int channel_x = 255 * abs(dx - temp_err) / e_dist;
				write_image_buffer(temp_x + x_step, y0, pack_color(channel_x, channel_x, channel_x), buffer);
			}
			err += dx;
			y0 += y_step;
		}
	}
}

image_buffer* image_buffer_create(uint32_t width, uint32_t height, int n_channels)
{
	image_buffer* p = malloc(sizeof* p);
	if(p == NULL)
		return NULL;
	p->width = width;
	p->height = height;
	p->n_channels = n_channels;
	p->buffer = calloc(width * height * n_channels, sizeof(uint8_t));
	if(p->buffer== NULL)
	{
		free(p);
		return NULL;
	}
	return p;
}

void image_buffer_destroy(image_buffer* p)
{
	free(p->buffer);
	free(p);
}

size_t image_buffer_size_b(image_buffer* p)
{
	return p->width * p->height * p->n_channels * sizeof(uint8_t);
}

static inline void write_image_buffer(int x, int y, uint32_t color, image_buffer* buffer)
{
	buffer->buffer[3 * (y * buffer->width + x) + 0] = (uint8_t)color;
	buffer->buffer[3 * (y * buffer->width + x) + 1] = (uint8_t)(color >> 8);
	buffer->buffer[3 * (y * buffer->width + x) + 2] = (uint8_t)(color >> 16);
}

static inline float lerp(float p0, float p1, float t)
{
	return p0 + (p1 - p0) * t;
}

static inline void clamp_edge_and_sort(int* x0, int* y0, int* x1, int* y1, image_buffer* buffer)
{
	if(*x0 > *x1)
		swap_ints(x0, x1);	
	if(*y0 > *y1)
		swap_ints(y0, y1);
	if(*x0 < 0)
		*x0 = 0;
	if(*y0 < 0)
		*y0 = 0;
	if(*x1 > buffer->width - 1)
		*x1 = buffer->width - 1;
	if(*y1 > buffer->height - 1)
		*y1 = buffer->height - 1;
}

static inline uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t c;
	c = r;
	c <<= 8;
	c |= g;
	c <<= 8;
	c |= b;
	return c;
}

static inline uint8_t get_color_component(uint32_t color, int component)
{
	return (uint8_t)(color >> (component << 8) & 0xff);
}

static inline uint32_t scale_color(uint32_t color, float t)
{
	float r = t * get_color_component(color, 0);
	float g = t * (float)get_color_component(color, 1);
	float b = t * (float)get_color_component(color, 2);
	return pack_color((uint8_t)(r), (uint8_t)(g), (uint8_t)(b));
}

static inline void swap_ints(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}










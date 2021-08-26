#ifndef SIMPLE_PPM_H
#define SIMPLE_PPM_H

#include <stdio.h>
#include <stdlib.h>

#define CREATOR "ThePi7on"
#define MAX_CHANNEL_VALUE 255
#define NOOP

/* Some RGB colors */
#define RGB_BLACK 0x000000
#define RGB_WHITE 0xFFFFFF
#define RGB_RED 0xFF0000
#define RGB_GREEN 0x00FF00
#define RGB_BLUE 0x0000FF
#define RGB_MAGENTA 0xFF00FF
#define RGB_YELLOW 0xFFFF00
#define RGB_CYAN 0x00FFFF

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} PPMPixel;

int index2Dto1D(int row_index, int col_index, int matrix_width);

PPMPixel *PPMPixel_create_ref(unsigned char r, unsigned char g, unsigned char b);
PPMPixel PPMPixel_create_val(unsigned char r, unsigned char g, unsigned char b);
void PPMPixel_set(PPMPixel *p, unsigned char r, unsigned char g, unsigned char b);

typedef PPMPixel PPMColor;
PPMColor *PPMColor_create_ref(unsigned int hexcol);
PPMColor PPMColor_create_val(unsigned int hexcol);

/* TODO: Add support for writing and reading 24 bit images */

typedef struct {
    unsigned int w;
    unsigned int h;
    PPMPixel *data;
} PPMImage;

PPMImage *PPMImage_create(unsigned int w, unsigned int h, PPMPixel *color);
PPMImage *PPMImage_read(const char *filename);
void PPMImage_write(const char *filename, PPMImage *img);

void PPMImage_draw_pixel(PPMImage *img, int px, int py, PPMColor color);
void PPMImage_draw_line(PPMImage *image, int x0, int y0, int x1, int y1, PPMColor color);
void PPMImage_draw_rect(PPMImage *image, int x, int y, int w, int h, PPMColor color, int filled);

PPMImage *PPM_resize_nearest(PPMImage *in, int out_width, int out_height);
PPMImage *PPM_resize_bilinear(PPMImage *in, int out_width, int out_height);

#endif /* SIMPLE_PPM_H */
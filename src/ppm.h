#ifndef SIMPLE_PPM_H
#define SIMPLE_PPM_H

#include <stdbool.h>

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

// NOTE: internally we work in BGRA, but for now we write and read PPM files only in RGB
#pragma warning(disable : 4201)  //unnamed union inside the PPMPixel struct
typedef struct {
    union {
        unsigned int val;  // 0x00RRGGBB
        struct {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char padding;  // alpha, but unused for now
        } chan;
    };
} PPMPixel;

void PPMPixel_swap(PPMPixel *a, PPMPixel *b);

typedef unsigned int PPMColor;
PPMColor PPMColor_compose(unsigned char r, unsigned char g, unsigned char b);

typedef struct {
    unsigned int w;
    unsigned int h;
    PPMPixel *data;
} PPMImage;

/** 
 * @param[in] w: Width.
 * @param[in] h: Height.
 * @param[in] color: Fill color.   
 */
PPMImage *PPMImage_create(unsigned int w, unsigned int h, PPMColor color);

void PPMImage_destroy(PPMImage *img);
PPMImage *PPMImage_read(const char *filename);
void PPMImage_write(const char *filename, PPMImage *img);

/**
 * @param[in] img: Image to draw on.
 * @param[in] px: X coordinate.
 * @param[in] py: Y coordinate.
 * @param[in] color: Pixel color.
 */
void PPMImage_draw_color(PPMImage *image, int px, int py, PPMColor color);
void PPMImage_draw_pixel(PPMImage *img, int px, int py, PPMPixel pix);
PPMPixel PPMImage_get_pixel(PPMImage *image, unsigned int x, unsigned int y);
void PPMImage_draw_line(PPMImage *image, int x0, int y0, int x1, int y1, PPMColor color);
void PPMImage_draw_rect(PPMImage *image, int x, int y, int w, int h, PPMColor color, bool filled);

PPMImage *PPMImage_crop(PPMImage *in, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

typedef enum {
    CHANNEL_BY_CHANNEL,
    WHITE_IF_DIFFERENT
} diff_mode;

PPMImage *PPMImage_diff(PPMImage *a, PPMImage *b, diff_mode mode);

PPMImage *PPM_resize_nearest(PPMImage *in, unsigned int w, unsigned int h);
PPMImage *PPM_descale_nearest(PPMImage *in, unsigned int assumed_w, unsigned int assumed_h);

#endif /* SIMPLE_PPM_H */
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
typedef struct {
    union {
        unsigned int val;  // 0x00RRGGBB
        struct {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char padding;  // alpha, but unused for now
        };
    };
} PPMPixel;

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
void PPMImage_draw_pixel(PPMImage *img, int px, int py, PPMColor color);
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

int clamp_int(int v, int min, int max);
double lerp_double(const double a, const double b, const double weight);

/**
 * Linearly interpolate two RGB pixels, channel by channel.
 *
 * @param[in] a: First pixel.
 * @param[in] b: Second pixel.
 * @param[in] weight: Weight used for the interpolation of each channel.
 * @param[in] round_flag: If set, rounds result for each color channel.  
 */
PPMPixel PPMPixel_lerp(PPMPixel a, PPMPixel b, const double weight, bool round_flag);

// Resources:
// - https://stackoverflow.com/questions/11991701/image-interpolation-bicubic-or-bilinear-what-if-there-are-no-neighbor-pixels
// - https://tinaja.com/glib/pixintpl.pdf
// - https://www.reddit.com/r/programming/comments/10c4w8/pure_javascript_html5_canvas_bilinear_image/c6ccwwn?utm_source=share&utm_medium=web2x&context=3
/**
 * @param[in] in: Input image.
 * @param[in] w: Target width.
 * @param[in] h: Target height.
 * @param[in] round_flag: Gets passed to PPMPixel_lerp(). If set applies rounding to interpolation results.
*/
PPMImage *PPM_resize_bilinear(PPMImage *in, unsigned int w, unsigned int h, bool round_flag);

#endif /* SIMPLE_PPM_H */
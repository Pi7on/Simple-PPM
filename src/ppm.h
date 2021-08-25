#include <stdio.h>
#include <stdlib.h>

#define CREATOR "ThePi7on"
#define RGB_COMPONENT_COLOR 255
#define NOOP

/* Some RGB colors */
#define RGB_BLACK 0x000000
#define RGB_WHITE 0xFFFFFF

#define RGB_RED 0xFF0000
#define RGB_GREEN 0x00FF00
#define RGB_BLUE 0x0000FF

#define RGB_MAGENTA (RGB_RED | RGB_BLUE)
#define RGB_YELLOW (RGB_RED | RGB_GREEN)
#define RGB_CYAN (RGB_BLUE | RGB_GREEN)

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} PPMPixel;

PPMPixel *PPMPixel_create_ref(unsigned char r, unsigned char g, unsigned char b);
PPMPixel PPMPixel_create_val(unsigned char r, unsigned char g, unsigned char b);
void PPMPixel_set(PPMPixel *p, unsigned char r, unsigned char g, unsigned char b);

typedef PPMPixel PPMColor;
PPMColor *PPMColor_create_ref(unsigned int hexcol);
PPMColor PPMColor_create_val(unsigned int hexcol);

typedef struct {
    unsigned int w;
    unsigned int h;
    PPMPixel *data;
} PPMImage;

PPMImage *PPMImage_create(unsigned int w, unsigned int h, PPMPixel *color);
PPMImage *PPMImage_read(const char *filename);
void PPMImage_write(const char *filename, PPMImage *img);

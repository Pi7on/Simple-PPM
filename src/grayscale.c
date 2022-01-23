#include "grayscale.h"

static const float r_scale = 0.299f;
static const float g_scale = 0.587f;
static const float b_scale = 0.114f;

void pixelGrayscale(PPMImage *input, unsigned int x, unsigned int y) {
    PPMPixel pixel = input->data[x + ((input->w) * y)];
    pixel.chan.r = pixel.chan.r * r_scale + pixel.chan.g * g_scale + pixel.chan.b * b_scale;
    pixel.chan.g = pixel.chan.r * r_scale + pixel.chan.g * g_scale + pixel.chan.b * b_scale;
    pixel.chan.b = pixel.chan.r * r_scale + pixel.chan.g * g_scale + pixel.chan.b * b_scale;
    input->data[x + ((input->w) * y)] = pixel;
}

void PPMImage_grayscale(PPMImage *input) {
    for (unsigned int y = 0; y < input->h; y++) {
        for (unsigned int x = 0; x < input->w; x++) {
            pixelGrayscale(input, x, y);
        }
    }
}

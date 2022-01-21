#include "grayscale.h"

void pixelGrayscale(PPMImage *input, unsigned int x, unsigned int y) {
    const float r_scale = 0.299f;
    const float g_scale = 0.587f;
    const float b_scale = 0.114f;

    PPMPixel pixel = input->data[x + ((input->w) * y)];
    pixel.chan.r = input->data[x + ((input->w) * y)].chan.r * r_scale + input->data[x + ((input->w) * y)].chan.g * g_scale + input->data[x + ((input->w) * y)].chan.b * b_scale;
    pixel.chan.g = input->data[x + ((input->w) * y)].chan.r * r_scale + input->data[x + ((input->w) * y)].chan.g * g_scale + input->data[x + ((input->w) * y)].chan.b * b_scale;
    pixel.chan.b = input->data[x + ((input->w) * y)].chan.r * r_scale + input->data[x + ((input->w) * y)].chan.g * g_scale + input->data[x + ((input->w) * y)].chan.b * b_scale;
    input->data[x + ((input->w) * y)] = pixel;
}

void imgGrayscale(PPMImage *input) {
    for (unsigned int y = 0; y < input->h; y++) {
        for (unsigned int x = 0; x < input->w; x++) {
            pixelGrayscale(input, x, y);
        }
    }
}

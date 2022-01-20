#include "dither.h"

#include <math.h>

#include "helpers.h"

PPMPixel quantize_pixel(float max, unsigned int steps, PPMPixel original) {
    PPMPixel output;
    output.chan.r = (unsigned char)round((steps * original.chan.r) / max) * (unsigned char)(max / steps);
    output.chan.g = (unsigned char)round((steps * original.chan.g) / max) * (unsigned char)(max / steps);
    output.chan.b = (unsigned char)round((steps * original.chan.b) / max) * (unsigned char)(max / steps);
    return output;
}

void addError(PPMImage *input, double factor, unsigned int x, unsigned int y, int errR, int errG, int errB) {
    if (x < 0 || x >= input->w || y < 0 || y >= input->h) {
        return;
    }

    PPMPixel pix = input->data[x + (input->w * y)];

    int new_r = pix.chan.r + errR * factor;
    int new_g = pix.chan.g + errG * factor;
    int new_b = pix.chan.b + errB * factor;

    pix.chan.r = CLAMP(new_r, 0, 255);
    pix.chan.g = CLAMP(new_g, 0, 255);
    pix.chan.b = CLAMP(new_b, 0, 255);

    input->data[x + ((input->w) * y)] = pix;
}

void distributeError(PPMImage *input, unsigned int x, unsigned int y, int errR, int errG, int errB) {
    addError(input, 7.0 / 16.0, x + 1, y + 0, errR, errG, errB);
    addError(input, 3.0 / 16.0, x - 1, y + 1, errR, errG, errB);
    addError(input, 5.0 / 16.0, x + 0, y + 1, errR, errG, errB);
    addError(input, 1.0 / 16.0, x + 1, y + 1, errR, errG, errB);
}

PPMImage *PPM_filter_FSDither(PPMImage *input, unsigned int steps) {
    PPMImage *out = PPMImage_create(input->w, input->h, 0);

    for (unsigned int y = 0; y < out->h; y++) {
        for (unsigned int x = 0; x < out->w; x++) {
            PPMPixel old_p = input->data[x + (input->w * y)];
            PPMPixel new_p = quantize_pixel(255.0, steps, old_p);
            input->data[x + (input->w * y)] = new_p;

            int errR = old_p.chan.r - new_p.chan.r;
            int errG = old_p.chan.g - new_p.chan.g;
            int errB = old_p.chan.b - new_p.chan.b;

            distributeError(input, x, y, errR, errG, errB);
        }
    }

    return input;
}
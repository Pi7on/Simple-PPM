#include "dither.h"
#include <math.h>

void addError(PPMImage *input, double factor, unsigned int x, unsigned int y, int errR, int errG, int errB) {
    if (x < 0 || x >= input->w || y < 0 || y >= input->h) return;

    PPMPixel clr = input->data[x + ((input->w) * y)];

    clr.chan.r = (clr.chan.r + errR * factor);
    clr.chan.g = (clr.chan.g + errG * factor);
    clr.chan.b = (clr.chan.b + errB * factor);

    input->data[x + ((input->w) * y)] = clr;
}

void distributeError(PPMImage *input, unsigned int x, unsigned int y, int errR, int errG, int errB) {
    addError(input, 5.0 / 16.0, x + 1, y, errR, errG, errB);
    addError(input, 1.5 / 16.0, x - 1, y + 1, errR, errG, errB);
    addError(input, 3.0 / 16.0, x, y + 1, errR, errG, errB);
    addError(input, 1.5 / 16.0, x + 1, y + 1, errR, errG, errB);
}

void pixelGrayscale(PPMImage *input, unsigned int x, unsigned int y){
    const float r_scale = 0.299;
    const float g_scale = 0.587;
    const float b_scale = 0.114;

    PPMPixel pixel = input->data[x + ((input->w) * y)];
    pixel.chan.r = input->data[x + ((input->w) * y)].chan.r * r_scale + input->data[x + ((input->w) * y)].chan.g * g_scale + input->data[x + ((input->w) * y)].chan.b * b_scale;
    pixel.chan.g = input->data[x + ((input->w) * y)].chan.r * r_scale + input->data[x + ((input->w) * y)].chan.g * g_scale + input->data[x + ((input->w) * y)].chan.b * b_scale;
    pixel.chan.b = input->data[x + ((input->w) * y)].chan.r * r_scale + input->data[x + ((input->w) * y)].chan.g * g_scale + input->data[x + ((input->w) * y)].chan.b * b_scale;
    input->data[x + ((input->w) * y)] = pixel;
}

void imgGrayscale(PPMImage *input){
    for (unsigned int y = 0; y < input->h; y++) {
        for (unsigned int x = 0; x < input->w; x++) {
            pixelGrayscale(input,x,y);
        }
    }
}

PPMImage *PPM_filter_FSDither(PPMImage *input) {
    for (unsigned int y = 0; y < input->h; y++) {
        for (unsigned int x = 0; x < input->w; x++) {
            PPMPixel old_p = input->data[x + ((input->w) * y)];

            PPMPixel new_p;
            unsigned char steps = 1;
            new_p.chan.r = lround((steps * old_p.chan.r) / 255.0) * floor(255.0 / steps);
            new_p.chan.g = lround((steps * old_p.chan.g) / 255.0) * floor(255.0 / steps);
            new_p.chan.b = lround((steps * old_p.chan.b) / 255.0) * floor(255.0 / steps);
            new_p.chan.padding = 0; 

            input->data[x + ((input->w) * y)] = new_p;

            int errR = old_p.chan.r - new_p.chan.r;
            int errG = old_p.chan.g - new_p.chan.g;
            int errB = old_p.chan.b - new_p.chan.b;

            distributeError(input, x, y, errR, errG, errB);
        }
    }

    imgGrayscale(input);
    return input;
}

#include "dither.h"

#include <math.h>

void addError(PPMImage *input, double factor, unsigned int x, unsigned int y, int errR, int errG, int errB) {
    if (x < 0 || x >= input->w || y < 0 || y >= input->h) {
        return;
    }

    PPMPixel clr = input->data[x + ((input->w) * y)];

    clr.chan.r = (clr.chan.r + errR * factor);
    clr.chan.g = (clr.chan.g + errG * factor);
    clr.chan.b = (clr.chan.b + errB * factor);

    input->data[x + ((input->w) * y)] = clr;
}

void distributeError(PPMImage *input, unsigned int x, unsigned int y, int errR, int errG, int errB) {
    addError(input, 7 / 16.0, x + 1, y, errR, errG, errB);
    addError(input, 3 / 16.0, x - 1, y + 1, errR, errG, errB);
    addError(input, 5 / 16.0, x, y + 1, errR, errG, errB);
    addError(input, 1 / 16.0, x + 1, y + 1, errR, errG, errB);
}

PPMImage *PPM_filter_FSDither(PPMImage *input) {
    PPMImage *out = PPMImage_create(input->w, input->h, 0);

    for (unsigned int y = 0; y < out->h; y++) {
        for (unsigned int x = 0; x < out->w; x++) {
            PPMPixel old_p = input->data[x + ((input->w) * y)];

            PPMPixel new_p;
            unsigned char steps = 1;
            new_p.chan.r = lround((steps * old_p.chan.r) / 255) * floor(255 / steps);
            new_p.chan.g = lround((steps * old_p.chan.g) / 255) * floor(255 / steps);
            new_p.chan.b = lround((steps * old_p.chan.b) / 255) * floor(255 / steps);

            input->data[x + ((input->w) * y)] = new_p;

            int errR = old_p.chan.r - new_p.chan.r;
            int errG = old_p.chan.g - new_p.chan.g;
            int errB = old_p.chan.b - new_p.chan.b;

            //Qualcosa non va. >:(
            distributeError(input, x, y, errR, errG, errB);
        }
    }

    return input;
}
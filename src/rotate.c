#include "rotate.h"

#include <stdio.h>
#include <stdlib.h>

#include "ppm.h"

void PPMImage_flip_horizontally(PPMImage *input) {
    for (unsigned int y = 0; y < input->h; y++) {
        for (unsigned int x = 0; x < (input->w / 2); x++) {
            PPMPixel *a = &(input->data[x + (y * input->w)]);
            PPMPixel *b = &(input->data[(input->w - x - 1) + (y * input->w)]);
            PPMPixel_swap(a, b);
        }
    }
}

void PPMImage_flip_vertically(PPMImage *input) {
    //TODO
}

PPMImage *PPMImage_rotate_90(PPMImage *input, int steps) {
    if (steps % 1 == 0) {
        //TODO rotate 90
    }

    if (steps % 2 == 0) {
        PPMImage *output = PPMImage_create(input->w, input->h, 0);
        output->data = input->data;
        PPMImage_flip_vertically(output);
        return output;
    }

    if (steps % 3 == 0) {
        //TODO rotate -90
    }

    if (steps % 4 == 0) {
        return input;
    }
}
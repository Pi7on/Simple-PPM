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
    int s = steps % 4;  // reduce steps to a value between 0 a 3
    switch (s) {
        case 0: {
            return input;
        }
        case 1: {
            //rotate 90
            //return
        }
        case 2: {
            //flip vertically
            //return
        }
        case 3: {
            //(flip vertically and rotate 90) OR (rotate -90), whichever turns out faster
            //return
        }
    }
}
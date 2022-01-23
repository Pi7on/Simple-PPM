#include "rotate.h"

#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
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
    for (unsigned int y = 0; y < (input->h / 2); y++) {
        for (unsigned int x = 0; x < input->w; x++) {
            PPMPixel *a = &(input->data[TWOD_ACCESS(x, y, input->w)]);
            PPMPixel *b = &(input->data[TWOD_ACCESS(x, (input->h - 1 - y), input->w)]);
            PPMPixel_swap(a, b);
        }
    }
}

PPMImage *PPMImage_rotate_clockwise(PPMImage *input) {
    PPMImage *output = PPMImage_create(input->h, input->w, 0);
    int new_y, new_x = 0;

    for (int y = input->h - 1; y >= 0; y--) {
        new_y = 0;
        for (int x = 0; x < input->w; x++) {
            output->data[x * output->w + y] = input->data[new_x * input->w + new_y];
            new_y++;
        }
        new_x++;
    }
    return output;
}

//roatates -90 deg
PPMImage *PPMImage_transpose(PPMImage *input) {
    PPMImage *output = PPMImage_create(input->h, input->w, 0);
    for (unsigned int y = 0; y < input->h; y++) {
        for (unsigned int x = 0; x < input->w; x++) {
            output->data[TWOD_ACCESS(y, x, output->w)] = input->data[TWOD_ACCESS(x, y, input->w)];
        }
    }
    return output;
}

PPMImage *PPMImage_rotate_90(PPMImage *input, int steps) {
    steps %= 4;  // reduce steps to a value between 0 a 3
    switch (steps) {
        case 0: {
            return input;
        }
        case 1: {
            PPMImage *output = PPMImage_rotate_clockwise(input);
            return output;
        }
        case 2: {
            PPMImage *output = PPMImage_copy(input);
            PPMImage_flip_vertically(output);
            return output;
        }
        case 3: {
            PPMImage *output = PPMImage_transpose(input);
            PPMImage_flip_horizontally(output);
            return output;
        }
        default: {
            //unreachable
            printf("something went very wrong.\n");
            exit(1);
        }
    }
}
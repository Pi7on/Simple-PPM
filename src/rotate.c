#include "rotate.h"
#include <stdio.h>

PPMImage *PPM_rotate_clockwise(PPMImage *input){

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

PPMImage *PPM_rotate_counterclockwise(PPMImage *input){
    //for the time beeing:
    return PPM_rotate_clockwise(PPM_rotate_clockwise(PPM_rotate_clockwise(input)));
}

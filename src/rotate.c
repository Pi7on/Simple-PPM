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

/*
https://stackoverflow.com/questions/49641245/how-to-rotate-a-c-sharp-mn-matrix-clockwise?noredirect=1&lq=1
https://stackoverflow.com/questions/29634857/rotating-a-ppm-image-90-degrees-to-the-right-in-c
*/
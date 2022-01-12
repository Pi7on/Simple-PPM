#include "dither.h"

PPMImage *PPM_filter_FSDither(PPMImage *input) {
    PPMImage *out = PPMImage_create(input->w, input->h, 0);
    return out;
}
#ifndef DITHER_H
#define DITHER_H

#include "ppm.h"

PPMImage *PPM_filter_FSDither(PPMImage *input, unsigned int target_bits_per_channel);

#endif  //DITHER_H
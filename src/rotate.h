#ifndef ROTATE_H
#define ROTATE_H

#include "ppm.h"
void PPMImage_flip_horizontally(PPMImage *input);
void PPMImage_flip_vertically(PPMImage *input);
PPMImage *PPMImage_rotate_90(PPMImage *input, int steps);

#endif  //ROTATE_H
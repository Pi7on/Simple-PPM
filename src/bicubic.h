#ifndef BICUBIC_H
#define BICUBIC_H

#include <stdio.h>
#include <stdlib.h>

#include "ppm.h"

double cubicInterpolate(double values[4], double x);

PPMPixel bilinearInterpolate(PPMImage* img, double x, double y);

PPMPixel bicubicInterpolate(PPMImage* img, double posX, double posY);

PPMPixel bicubicInterpolateBuffered(PPMImage* img, double posX, double posY);

PPMImage* resize_bicubic(PPMImage* src, unsigned int target_width,
                         unsigned int target_height);

#endif /* BICUBIC_H */

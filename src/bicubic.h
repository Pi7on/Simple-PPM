#ifndef BICUBIC_H
#define BICUBIC_H

#include <stdint.h>

#include "ppm.h"

float cubic_hermite(float A, float B, float C, float D, float t);

void get_pixel_clamped(PPMImage *source_image, int x, int y, uint8_t *temp_pixel);

void sample_bicubic(PPMImage *source_image, float u, float v, uint8_t *sample);

void resize_bicubic(PPMImage *source_image, PPMImage *destination_image, float scale);

#endif /* BICUBIC_H */
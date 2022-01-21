#include "dither.h"
#include "grayscale.h"
#include "ppm.h"
#include "rotate.h"
#include "stdio.h"

#define DATAPATH "..\\..\\data\\"

int main() {
    PPMImage *input = PPMImage_read(DATAPATH "2b.ppm");
    imgGrayscale(input);
    PPMImage *output = PPMImage_transform_rotate90(input, 2);
    PPMImage_write(DATAPATH "2b_out.ppm", PPM_filter_FSDither(output, 2));
    return 0;
}
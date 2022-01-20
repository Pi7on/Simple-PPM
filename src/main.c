#include "dither.h"
#include "ppm.h"
#include <stdio.h>

#define DATAPATH "..\\..\\data\\"

int main() {
    PPMImage *input = PPMImage_read(DATAPATH "2b.ppm");
    PPMImage *output = PPM_filter_FSDither(input, 1);
    PPMImage_write(DATAPATH "2b_dither.ppm", output);
    return 0;
}

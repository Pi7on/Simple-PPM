#include "bicubic.h"

#include "ppm.h"

float cubic_hermite(float A, float B, float C, float D, float t) {
    float a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
    float b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
    float c = -A / 2.0f + C / 2.0f;
    float d = B;

    return (a * t * t * t +
            b * t * t +
            c * t + d);
}

void get_pixel_clamped(PPMImage *source_image, int x, int y, uint8_t *temp_pixel) {
    int cx = clamp_int(x, 0, source_image->w - 1);
    int cy = clamp_int(y, 0, source_image->h - 1);

    // printf("cy: %d - cx: %d\n", cy, cx);

    temp_pixel[0] = source_image->data[cx + (source_image->w * cy)].r;
    temp_pixel[1] = source_image->data[cx + (source_image->w * cy)].g;
    temp_pixel[2] = source_image->data[cx + (source_image->w * cy)].b;
}

void sample_bicubic(PPMImage *source_image, float u, float v, uint8_t *sample) {
    float x = (u * source_image->w) - 0.5f;
    int xint = (int)x;
    float xfract = x - xint;

    float y = (v * source_image->h) - 0.5f;
    int yint = (int)y;
    float yfract = y - yint;

    uint8_t p00[3];
    uint8_t p10[3];
    uint8_t p20[3];
    uint8_t p30[3];

    uint8_t p01[3];
    uint8_t p11[3];
    uint8_t p21[3];
    uint8_t p31[3];

    uint8_t p02[3];
    uint8_t p12[3];
    uint8_t p22[3];
    uint8_t p32[3];

    uint8_t p03[3];
    uint8_t p13[3];
    uint8_t p23[3];
    uint8_t p33[3];

    // 1st row
    get_pixel_clamped(source_image, xint - 1, yint - 1, p00);
    get_pixel_clamped(source_image, xint + 0, yint - 1, p10);
    get_pixel_clamped(source_image, xint + 1, yint - 1, p20);
    get_pixel_clamped(source_image, xint + 2, yint - 1, p30);

    // 2nd row
    get_pixel_clamped(source_image, xint - 1, yint + 0, p01);
    get_pixel_clamped(source_image, xint + 0, yint + 0, p11);
    get_pixel_clamped(source_image, xint + 1, yint + 0, p21);
    get_pixel_clamped(source_image, xint + 2, yint + 0, p31);

    // 3rd row
    get_pixel_clamped(source_image, xint - 1, yint + 1, p02);
    get_pixel_clamped(source_image, xint + 0, yint + 1, p12);
    get_pixel_clamped(source_image, xint + 1, yint + 1, p22);
    get_pixel_clamped(source_image, xint + 2, yint + 1, p32);

    // 4th row
    get_pixel_clamped(source_image, xint - 1, yint + 2, p03);
    get_pixel_clamped(source_image, xint + 0, yint + 2, p13);
    get_pixel_clamped(source_image, xint + 1, yint + 2, p23);
    get_pixel_clamped(source_image, xint + 2, yint + 2, p33);

    // perform interpolation on each row
    for (int i = 0; i < 3; i++) {
        float col0 = cubic_hermite(p00[i], p10[i], p20[i], p30[i], xfract);
        float col1 = cubic_hermite(p01[i], p11[i], p21[i], p31[i], xfract);
        float col2 = cubic_hermite(p02[i], p12[i], p22[i], p32[i], xfract);
        float col3 = cubic_hermite(p03[i], p13[i], p23[i], p33[i], xfract);

        // perform interpolation on the 4 values obtained from previous step
        float value = cubic_hermite(col0, col1, col2, col3, yfract);

        value = clamp_float(value, 0.0f, 255.0f);

        sample[i] = (uint8_t)value;
    }
}

void resize_image(PPMImage *source_image, PPMImage *destination_image, float scale) {
    uint8_t sample[3];

    destination_image->w = (unsigned int)((float)(source_image->w) * scale);
    destination_image->h = (unsigned int)((float)(source_image->h) * scale);

    //printf("x-width=%d | y-width=%d\n", destination_image->w, destination_image->h);

    for (int y = 0; y < destination_image->h; y++) {
        float v = (float)y / (float)(destination_image->h);
        for (int x = 0; x < destination_image->w; x++) {
            float u = (float)x / (float)(destination_image->w);

            //printf("v: %f - u: %f\n", v, u);
            sample_bicubic(source_image, u, v, sample);

            destination_image->data[x + ((destination_image->w) * y)].r = sample[0];
            destination_image->data[x + ((destination_image->w) * y)].g = sample[1];
            destination_image->data[x + ((destination_image->w) * y)].b = sample[2];
        }
    }
}

void resize_bicubic(PPMImage *source_image, PPMImage *destination_image, float scale) {
    uint8_t sample[3];

    destination_image->w = (unsigned int)((float)(source_image->w) * scale);
    destination_image->h = (unsigned int)((float)(source_image->h) * scale);

    for (int y = 0; y < destination_image->h; y++) {
        float v = (float)y / (float)(destination_image->h);
        for (int x = 0; x < destination_image->w; x++) {
            float u = (float)x / (float)(destination_image->w);

            sample_bicubic(source_image, u, v, sample);

            destination_image->data[x + ((destination_image->w) * y)].r = sample[0];
            destination_image->data[x + ((destination_image->w) * y)].g = sample[1];
            destination_image->data[x + ((destination_image->w) * y)].b = sample[2];
        }
    }
}
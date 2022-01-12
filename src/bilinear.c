#include "bilinear.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

void get_pixel_clamped_bilinear(PPMImage *source_image, int x, int y, PPMPixel *destination_pixel) {
    // TODO: is this cast always safe?
    //                        V
    int cx = CLAMP(x, 0, (signed int)source_image->w - 1);
    int cy = CLAMP(y, 0, (signed int)source_image->h - 1);

    *destination_pixel = source_image->data[cx + (source_image->w * cy)];
}

PPMPixel PPMPixel_lerp(PPMPixel a, PPMPixel b, const double weight, bool do_round) {
    if (a.val == b.val) {
        return a;
    }

    PPMPixel ret;
    // TODO: do I need to clamp the result of the interpolation between 0.0f and 255.0f ?
    if (do_round) {
        ret.chan.r = (unsigned char)round(LERP(a.chan.r, b.chan.r, weight));
        ret.chan.g = (unsigned char)round(LERP(a.chan.g, b.chan.g, weight));
        ret.chan.b = (unsigned char)round(LERP(a.chan.b, b.chan.b, weight));
        //NOTE: add alpha channel here when implemented
    } else {
        ret.chan.r = (unsigned char)LERP(a.chan.r, b.chan.r, weight);
        ret.chan.g = (unsigned char)LERP(a.chan.g, b.chan.g, weight);
        ret.chan.b = (unsigned char)LERP(a.chan.b, b.chan.b, weight);
        //NOTE: add alpha channel here when implemented
    }

    return ret;
}

void sample_bilinear(PPMImage *source_image, float u, float v, PPMPixel *sample) {
    float x = (u * source_image->w) - 0.5f;
    int xint = (int)floor(x);
    float xfract = x - xint;

    float y = (v * source_image->h) - 0.5f;
    int yint = (int)floor(y);
    float yfract = y - yint;

    PPMPixel p00;
    PPMPixel p01;
    PPMPixel p10;
    PPMPixel p11;

    //printf("y: %f - x: %f --- yint: %d - xint:%d\n", y, x, yint, xint);

    // 1st row
    get_pixel_clamped_bilinear(source_image, xint + 0, yint + 0, &p00);
    get_pixel_clamped_bilinear(source_image, xint + 1, yint + 0, &p10);

    // 2nd row
    get_pixel_clamped_bilinear(source_image, xint + 0, yint + 1, &p01);
    get_pixel_clamped_bilinear(source_image, xint + 1, yint + 1, &p11);

    // perform interpolation on each row
    for (int i = 0; i < 3; i++) {
        PPMPixel col0 = PPMPixel_lerp(p00, p10, xfract, false);
        PPMPixel col1 = PPMPixel_lerp(p01, p11, xfract, false);

        // perform interpolation on the 4 values obtained from previous step
        PPMPixel final = PPMPixel_lerp(col0, col1, yfract, false);

        *sample = final;
    }
}

PPMImage *PPM_resize_bilinear(PPMImage *in, unsigned int w, unsigned int h, bool round_flag) {
    if (!in) {
        fprintf(stderr, "%s: PPM_resize_bilinear received null image as input.\n", __func__);
        exit(1);
    }

    // We don't need to resize
    if (in->w == w && in->h == h) {
        return in;
    }
    // TODO: use CLAMP
    if (w < 1) {
        fprintf(stderr, "%s: width was set to %d. It will be set to 1.\n", __func__, w);
        w = 1;
    }
    // TODO: use CLAMP
    if (h < 1) {
        fprintf(stderr, "%s: height was set to %d. It will be set to 1.\n", __func__, h);
        h = 1;
    }

    PPMImage *out = PPMImage_create(w, h, 0);
    if (!out) {
        fprintf(stderr, "%s: Allocation of output image failed.\n", __func__);
        exit(1);
    }

    float scale_factor_w = (float)w / (float)in->w;
    float scale_factor_h = (float)h / (float)in->h;

    out->w = (unsigned int)((float)(in->w) * scale_factor_w);
    out->h = (unsigned int)((float)(in->h) * scale_factor_h);

    PPMPixel sample;

    for (unsigned int y = 0; y < out->h; y++) {
        float v = (float)y / (float)(out->h);
        for (unsigned int x = 0; x < out->w; x++) {
            float u = (float)x / (float)(out->w);

            sample_bilinear(in, u, v, &sample);

            out->data[x + ((out->w) * y)] = sample;
        }
    }
    return out;
}

#include "bilinear.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

PPMPixel PPMPixel_lerp(PPMPixel a, PPMPixel b, const double weight, bool do_round) {
    if (a.val == b.val) {
        return a;
    }

    PPMPixel ret;

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

PPMImage *PPM_resize_bilinear(PPMImage *in, unsigned int w, unsigned int h, bool round_flag) {
    if (!in) {
        fprintf(stderr, "PPM_resize_bilinear received null image as input.\n");
        exit(1);
    }
    PPMImage *out = PPMImage_create(w, h, 0);

    for (unsigned int cy = 0; cy < out->h; cy++) {               // cy: current y position on output image
        const double v = ((double)cy) / ((double)(out->h));      // v: current position on the output's Y axis (in percentage)
        for (unsigned int cx = 0; cx < out->w; cx++) {           // cx: current x position on output image
            const double u = ((double)cx) / ((double)(out->w));  // u: current position on the output's X axis (in percentage)

            const double y_double = (in->h * v) - 0.5;  //
            const double x_double = (in->w * u) - 0.5;  // UV map input coordinates to output coordinates (both go from 0 to 1)

            const double y_weight = y_double - floor(y_double);  //
            const double x_weight = x_double - floor(x_double);  //  interpolation weights

            const int y_int = (int)floor(y_double);  //
            const int x_int = (int)floor(x_double);  // floor output's UV mapped coordinates to "snap" them to an actual pixel of the input image

            // NOTE: we're clamping x and y so we don't go sampling outside the bounds of the input image.
            PPMPixel sample_tl = in->data[CLAMP(y_int, 0, (signed int)in->h) * in->w + CLAMP(x_int, 0, (signed int)in->w)];                  // top left sample
            PPMPixel sample_tr = in->data[CLAMP(y_int, 0, (signed int)in->h) * in->w + CLAMP(x_int + 1, 0, (signed int)in->w - 1)];          // top right sample
            PPMPixel sample_bl = in->data[CLAMP(y_int + 1, 0, (signed int)in->h - 1) * in->w + CLAMP(x_int, 0, (signed int)in->w)];          // bottom left sample
            PPMPixel sample_br = in->data[CLAMP(y_int + 1, 0, (signed int)in->h - 1) * in->w + CLAMP(x_int + 1, 0, (signed int)in->w - 1)];  // bottom right sample

            PPMPixel lerp_t = PPMPixel_lerp(sample_tl, sample_tr, x_weight, round_flag);  // interpolation between top left sample and top right sample
            PPMPixel lerp_b = PPMPixel_lerp(sample_bl, sample_br, x_weight, round_flag);  // interpolation between bottom left sample and bottom right sample

            PPMPixel lerp_final = PPMPixel_lerp(lerp_t, lerp_b, y_weight, round_flag);  // interpolation between results of the two previuos interpolations

            out->data[cy * out->w + cx] = lerp_final;
        }
    }

    // SECOND PASS
    // same procedure, but use previous state of the output as input.
    // This second pass doesn't change the size, it just fixes the half-pixel shift caused by the first pass.
    // TODO: find out if there's a better way than this one.
    /*
    for (unsigned int cy = 0; cy < out->h; cy++) {
        const double v = ((double)cy) / ((double)(out->h));
        for (unsigned int cx = 0; cx < out->w; cx++) {
            const double u = ((double)cx) / ((double)(out->w));

            const double y_double = (out->h * v) + 0.5;
            const double x_double = (out->w * u) + 0.5;

            const double y_weight = y_double - floor(y_double);
            const double x_weight = x_double - floor(x_double);

            const int y_int = (int)floor(y_double);
            const int x_int = (int)floor(x_double);

            PPMPixel sample_tl = out->data[clamp_int(y_int, 0, out->h) * out->w + clamp_int(x_int, 0, out->w)];
            PPMPixel sample_tr = out->data[clamp_int(y_int, 0, out->h) * out->w + clamp_int(x_int + 1, 0, out->w - 1)];
            PPMPixel sample_bl = out->data[clamp_int(y_int + 1, 0, out->h - 1) * out->w + clamp_int(x_int, 0, out->w)];
            PPMPixel sample_br = out->data[clamp_int(y_int + 1, 0, out->h - 1) * out->w + clamp_int(x_int + 1, 0, out->w - 1)];

            PPMPixel lerp_t = PPMPixel_lerp(sample_tl, sample_tr, x_weight, round_flag);
            PPMPixel lerp_b = PPMPixel_lerp(sample_bl, sample_br, x_weight, round_flag);

            PPMPixel lerp_final = PPMPixel_lerp(lerp_t, lerp_b, y_weight, round_flag);

            out->data[cy * out->w + cx] = lerp_final;
        }
    }*/

    return out;
}

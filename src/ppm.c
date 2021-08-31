#include "ppm.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

PPMColor PPMColor_compose(unsigned char r, unsigned char g, unsigned char b) {
    PPMColor ret = 0;  //shouldn't C initialize to zero? (bacause if I don't, I get wrong colors)
    ret = ret | (((unsigned int)b) << (8 * 0));
    ret = ret | (((unsigned int)g) << (8 * 1));
    ret = ret | (((unsigned int)r) << (8 * 2));
    ret = ret | (((unsigned int)0) << (8 * 3));  // most significant byte is unused for now
    return ret;
}

PPMImage *PPMImage_create(unsigned int w, unsigned int h, PPMColor color) {
    PPMImage *ret = calloc(1, sizeof(PPMImage));
    ret->w = w;
    ret->h = h;
    ret->data = calloc(w * h, sizeof(PPMPixel));

    if (color) {
        for (int i = 0; i < w * h; i++) {
            ret->data[i].val = color;
        }
    } else {
        for (int i = 0; i < w * h; i++) {
            ret->data[i].val = color;
        }
    }
    return ret;
}

void PPMImage_destroy(PPMImage *img) {
    // don't need to check if img == NULL, free already handles it.
    // https://stackoverflow.com/questions/6084218/is-it-good-practice-to-free-a-null-pointer-in-c/6084233
    free(img->data);
    free(img);
}

PPMImage *PPMImage_read(const char *filename) {
    const char PPM_MAGIC[2] = "P6";
    char buff[16];
    PPMImage *img;
    FILE *fp;
    int c;
    int rgb_comp_color;

    //open PPM file for reading
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "%s: Unable to open file '%s'\n", __func__, filename);
        exit(1);
    }

    //read image format
    if (!fgets(buff, sizeof(buff), fp)) {
        perror(filename);
        exit(1);
    }

    //check the image format usign magic bytes
    if (buff[0] != PPM_MAGIC[0] || buff[1] != PPM_MAGIC[1]) {
        fprintf(stderr, "%s: Invalid image format (must be 'P6')\n", __func__);
        exit(1);
    }

    //alloc memory form image
    img = (PPMImage *)calloc(1, sizeof(PPMImage));
    if (!img) {
        fprintf(stderr, "%s: Unable to allocate memory\n", __func__);
        exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
        while (getc(fp) != '\n') {
            NOOP;
        }
        c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", &img->w, &img->h) != 2) {
        fprintf(stderr, "%s: Invalid image size (error loading '%s')\n", __func__, filename);
        exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
        fprintf(stderr, "%s: Invalid rgb component (error loading '%s')\n", __func__, filename);
        exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color != MAX_CHANNEL_VALUE) {
        fprintf(stderr, "%s: '%s' does not have 8-bits components\n", __func__, filename);
        exit(1);
    }

    while (fgetc(fp) != '\n') {
        NOOP;
    }

    //memory allocation for pixel data
    img->data = (PPMPixel *)calloc(img->w * img->h, sizeof(PPMPixel));

    if (!img) {
        fprintf(stderr, "%s: Unable to allocate memory\n", __func__);
        exit(1);
    }

    //read pixel data
    for (int i = 0; i < img->w * img->h; i++) {
        unsigned char rgb[3] = {0};
        fread(&rgb, (sizeof(rgb)), 1, fp);

        img->data[i].r = rgb[0];
        img->data[i].g = rgb[1];
        img->data[i].b = rgb[2];
    }
    ///////////////////
    fclose(fp);
    return img;
}

void PPMImage_write(const char *filename, PPMImage *img) {
    if (!img) {
        fprintf(stderr, "%s: PPMImage_write received NULL input for '%s'\n", __func__, filename);
    }

    FILE *fp;

    fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "%s: Unable to open file '%s'\n", __func__, filename);

        exit(1);
    }

    //write header magic
    fprintf(fp, "P6\n");

    //comments
    fprintf(fp, "# Created by %s\n", CREATOR);

    //image size
    fprintf(fp, "%d %d\n", img->w, img->h);

    // rgb max value per channel
    fprintf(fp, "%d\n", MAX_CHANNEL_VALUE);

    // pixel data
    for (int i = 0; i < img->w * img->h; i++) {
        PPMPixel *bgra = &(img->data[i]);

        unsigned char rgb[3] = {bgra->r, bgra->g, bgra->b};
        fwrite(&rgb, (sizeof(rgb)), 1, fp);
    }

    fclose(fp);
}

void PPMImage_draw_pixel(PPMImage *img, int px, int py, PPMColor color) {
    img->data[py * img->w + px].val = color;
}

void PPMImage_draw_line(PPMImage *image, int x0, int y0, int x1, int y1, PPMColor color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        PPMImage_draw_pixel(image, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void PPMImage_draw_rect(PPMImage *image, int x, int y, int w, int h, PPMColor color, bool filled) {
    if (filled) {
        for (int i = 0; i < h; i++) {
            PPMImage_draw_line(image, x, y + i, x + w, y + i, color);
        }
    } else {
        PPMImage_draw_line(image, x, y, x + w, y, color);
        PPMImage_draw_line(image, x + w, y, x + w, y + h, color);
        PPMImage_draw_line(image, x + w, y + h, x, y + h, color);
        PPMImage_draw_line(image, x, y + h, x, y, color);
    }
}

PPMImage *PPMImage_crop(PPMImage *in, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom) {
    int new_w = in->w - left - right;
    int new_h = in->h - top - bottom;

    if (new_w <= 0 || new_h <= 0) {
        fprintf(stderr, "%s: resulting image would have an invalid size.\n", __func__);
        exit(1);
    }

    PPMImage *out = PPMImage_create(new_w, new_h, 0);

    for (unsigned int y = 0; y < out->h; y++) {
        for (unsigned int x = 0; x < out->w; x++) {
            out->data[y * out->w + x] = in->data[(y + top) * in->w + (x + left)];
        }
    }

    return out;
}

PPMImage *PPMImage_diff(PPMImage *a, PPMImage *b, diff_mode mode) {
    // both must have the same dimensions
    if (!a || !b) {
        fprintf(stderr, "one of the inputs is NULL.\n");
        exit(1);
    }
    if ((a->w != b->w) || (a->h != b->h)) {
        fprintf(stderr, "images must have the same dimensions.\n");
        exit(1);
    }

    PPMImage *diff = PPMImage_create(a->w, a->h, 0);
    for (int y = 0; y < a->h; y++) {
        for (int x = 0; x < a->w; x++) {
            diff->data[y * diff->w + x].r = (unsigned char)abs(a->data[y * a->w + x].r - b->data[y * b->w + x].r);
            diff->data[y * diff->w + x].g = (unsigned char)abs(a->data[y * a->w + x].g - b->data[y * b->w + x].g);
            diff->data[y * diff->w + x].b = (unsigned char)abs(a->data[y * a->w + x].b - b->data[y * b->w + x].b);

            switch (mode) {
                default: {
                    // fallthrough
                }
                case CHANNEL_BY_CHANNEL: {
                    break;
                }
                case WHITE_IF_DIFFERENT: {
                    if (diff->data[y * diff->w + x].r > 0x00 ||
                        diff->data[y * diff->w + x].g > 0x00 ||
                        diff->data[y * diff->w + x].b > 0x00) {
                        diff->data[y * diff->w + x].val = 0xFFFFFF;
                    }
                    break;
                }
            }
        }
    }
    return diff;
}

void PPM_resize_nearest(PPMImage *in, PPMImage *out) {
    if (!in) {
        fprintf(stderr, "PPM_resize_nearest received null image as input.\n");
        exit(1);
    }
    if (!out) {
        fprintf(stderr, "PPM_resize_nearest received null image as output.\n");
        exit(1);
    }
    //TODO: add 0.5 pixel in upscalig?
    // ffmpeg does it: https://github.com/FFmpeg/FFmpeg/blob/bc70684e74a185d7b80c8b80bdedda659cb581b8/libavfilter/transform.c

    // TODO: use double when calculating "u" and "v" ???
    for (unsigned int y_out = 0; y_out < out->h; y_out++) {
        const float v = ((float)y_out) / ((float)(out->h));  // v: current position on the output's Y axis (in percentage)
        for (unsigned int x_out = 0; x_out < out->w; x_out++) {
            const float u = ((float)x_out) / ((float)(out->w));  // u: current position on the output's X axis (in percentage)

            // "nearest" should round
            // "integer" should floor (or cast to int).
            // So technically, this isn't actually Nearest Neighbour?

            const int x_in = (int)(in->w * u);
            const int y_in = (int)(in->h * v);

            out->data[y_out * out->w + x_out] = in->data[y_in * in->w + x_in];
        }
    }
}

PPMImage *PPM_descale_nearest(PPMImage *in, unsigned int assumed_w, unsigned int assumed_h) {
    PPMImage *out = PPMImage_create(assumed_w, assumed_h, 0);
    double half_pixel_offest = 0.5;
    for (unsigned int y_out = 0; y_out < out->h; y_out++) {
        const double v = ((double)y_out) / ((double)(out->h));  // v: current position on the output's Y axis (in percentage)
        for (unsigned int x_out = 0; x_out < out->w; x_out++) {
            const double u = ((double)x_out) / ((double)(out->w));  // u: current position on the output's X axis (in percentage)

            /* TODO: add explanation for why we need this half-pixel offset */
            const int x_in = round(in->w * u + half_pixel_offest);
            const int y_in = round(in->h * v + half_pixel_offest);

            out->data[y_out * out->w + x_out] = in->data[y_in * in->w + x_in];
        }
    }
    return out;
}

int clamp_int(int v, int min, int max) {
    if (v < min) {
        return min;
    }
    if (v > max) {
        return max;
    }
    return v;
}

double lerp_double(const double a, const double b, const double weight) {
    //TODO: test if this check slows me down
    if (a == b) {
        return a;
    }
    return (a * ((double)1.0 - weight) + (b * weight));
}

PPMPixel PPMPixel_lerp(PPMPixel a, PPMPixel b, const double weight, bool do_round) {
    if (a.val == b.val) {
        return a;
    }

    PPMPixel ret;

    if (do_round) {
        ret.r = (unsigned char)round(lerp_double(a.r, b.r, weight));
        ret.g = (unsigned char)round(lerp_double(a.g, b.g, weight));
        ret.b = (unsigned char)round(lerp_double(a.b, b.b, weight));
        //NOTE: add alpha channel here when implemented
    } else {
        ret.r = (unsigned char)lerp_double(a.r, b.r, weight);
        ret.g = (unsigned char)lerp_double(a.g, b.g, weight);
        ret.b = (unsigned char)lerp_double(a.b, b.b, weight);
        //NOTE: add alpha channel here when implemented
    }

    return ret;
}

/*
 Resources:
    - https://stackoverflow.com/questions/11991701/image-interpolation-bicubic-or-bilinear-what-if-there-are-no-neighbor-pixels
    - https://tinaja.com/glib/pixintpl.pdf
    - https://www.reddit.com/r/programming/comments/10c4w8/pure_javascript_html5_canvas_bilinear_image/c6ccwwn?utm_source=share&utm_medium=web2x&context=3
*/

// TODO: fix half-pixel shift in the output (compared to ffmpeg and affinity photo)
void PPM_resize_bilinear(PPMImage *in, PPMImage *out, bool round_flag) {
    if (!in) {
        fprintf(stderr, "PPM_resize_bilinear received null image as input.\n");
        exit(1);
    }
    if (!out) {
        fprintf(stderr, "PPM_resize_bilinear received null image as output.\n");
        exit(1);
    }

    for (unsigned int cy = 0; cy < out->h; cy++) {               // cy: current y position on output image
        const double v = ((double)cy) / ((double)(out->h));      // v: current position on the output's Y axis (in percentage)
        for (unsigned int cx = 0; cx < out->w; cx++) {           // cx: current x position on output image
            const double u = ((double)cx) / ((double)(out->w));  // u: current position on the output's X axis (in percentage)

            // printf("cy: %u, cx: %u\n", cy, cx);
            //printf("v: %0.15f, u: %0.15f\n", v, u);

            const double y_double = (in->h * v) - 0.5;  //
            const double x_double = (in->w * u) - 0.5;  // UV map input coordinates to output coordinates (both go from 0 to 1)

            //printf("y_double: %f, x_double: %f\n", y_double, x_double);

            const double y_weight = y_double - floor(y_double);  //
            const double x_weight = x_double - floor(x_double);  //  interpolation weights

            //printf("y_weight: %f, x_weight: %f\n", y_weight, x_weight);

            const int y_int = (int)floor(y_double);  //
            const int x_int = (int)floor(x_double);  // floor output's UV mapped coordinates to "snap" them to an actual pixel of the input image

            //printf("y_int: %d, x_int: %d\n", y_int, x_int);

            // NOTE: we're clamping x and y so we don't go sampling outside the bounds of the input image.
            // NOTE2: TGA lerpa per colonne, mentre qui lerpo per righe. prova a cambiare.
            // NOTA3: differenze se tolgo quel -1 a in TUTTI i calmp?
            PPMPixel sample_tl = in->data[clamp_int(y_int, 0, in->h) * in->w + clamp_int(x_int, 0, in->w)];                  // top left sample
            PPMPixel sample_tr = in->data[clamp_int(y_int, 0, in->h) * in->w + clamp_int(x_int + 1, 0, in->w - 1)];          // top right sample
            PPMPixel sample_bl = in->data[clamp_int(y_int + 1, 0, in->h - 1) * in->w + clamp_int(x_int, 0, in->w)];          // bottom left sample
            PPMPixel sample_br = in->data[clamp_int(y_int + 1, 0, in->h - 1) * in->w + clamp_int(x_int + 1, 0, in->w - 1)];  // bottom right sample

            //printf("samp_tl - pos: [y: %d x: %d], val: [%u %d %u]\n", clamp_int(y_int, 0, in->h), clamp_int(x_int, 0, in->w), sample_tl.r, sample_tl.g, sample_tl.b);
            //printf("samp_tr - pos: [y: %d x: %d], val: [%u %d %u]\n", clamp_int(y_int, 0, in->h), clamp_int(x_int + 1, 0, in->w - 1), sample_tr.r, sample_tr.g, sample_tr.b);
            //printf("samp_bl - pos: [y: %d x: %d], val: [%u %d %u]\n", clamp_int(y_int + 1, 0, in->h - 1), clamp_int(x_int, 0, in->w), sample_bl.r, sample_bl.g, sample_bl.b);
            //printf("samp_br - pos: [y: %d x: %d], val: [%u %d %u]\n", clamp_int(y_int + 1, 0, in->h - 1), clamp_int(x_int + 1, 0, in->w - 1), sample_br.r, sample_br.g, sample_br.b);

            PPMPixel lerp_t = PPMPixel_lerp(sample_tl, sample_tr, x_weight, round_flag);  // interpolation between top left sample and top right sample
            PPMPixel lerp_b = PPMPixel_lerp(sample_bl, sample_br, x_weight, round_flag);  // interpolation between bottom left sample and bottom right sample

            //printf("lerp_t - val: [%u %d %u]\n", lerp_t.r, lerp_t.g, lerp_t.b);
            //printf("lerp_b - val: [%u %d %u]\n", lerp_b.r, lerp_b.g, lerp_b.b);

            PPMPixel lerp_final = PPMPixel_lerp(lerp_t, lerp_b, y_weight, round_flag);  // interpolation between results of the two previuos interpolations

            //printf("lerp_fin - val: [%u %d %u]\n\n", lerp_final.r, lerp_final.g, lerp_final.b);

            out->data[cy * out->w + cx] = lerp_final;
        }
    }
}

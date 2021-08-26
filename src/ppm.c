#include "ppm.h"

#include <stdio.h>
#include <stdlib.h>

int index2Dto1D(int row_index, int col_index, int matrix_width) {
    return (matrix_width * row_index) + col_index;
}

PPMPixel *PPMPixel_create_ref(unsigned char r, unsigned char g, unsigned char b) {
    PPMPixel *ret = malloc(sizeof(PPMPixel));
    ret->r = r;
    ret->g = g;
    ret->b = b;
    return ret;
}

PPMPixel PPMPixel_create_val(unsigned char r, unsigned char g, unsigned char b) {
    PPMPixel ret;
    ret.r = r;
    ret.g = g;
    ret.b = b;
    return ret;
}

void PPMPixel_set(PPMPixel *p, unsigned char r, unsigned char g, unsigned char b) {
    p->r = r;
    p->g = g;
    p->b = b;
}

PPMPixel *PPMColor_create_ref(unsigned int hexcol) {
    // unsigned int --> 32bit --> 4 bytes --> 00000000 RRRRRRRR GGGGGGGG BBBBBBBB

    unsigned char r = (unsigned char)(hexcol >> 16);
    unsigned char g = (unsigned char)(hexcol >> 8);
    unsigned char b = (unsigned char)(hexcol);

    PPMPixel *ret = PPMPixel_create_ref(r, g, b);
    return ret;
}

PPMPixel PPMColor_create_val(unsigned int hexcol) {
    // unsigned int --> 32bit --> 4 bytes --> 00000000 RRRRRRRR GGGGGGGG BBBBBBBB

    unsigned char r = (unsigned char)(hexcol >> 16);
    unsigned char g = (unsigned char)(hexcol >> 8);
    unsigned char b = (unsigned char)(hexcol);

    PPMPixel ret;
    ret.r = r;
    ret.g = g;
    ret.b = b;
    return ret;
}

PPMImage *PPMImage_create(unsigned int w, unsigned int h, PPMPixel *color) {
    PPMImage *ret = malloc(sizeof(PPMImage));
    ret->w = w;
    ret->h = h;
    ret->data = malloc(w * h * sizeof(PPMPixel));

    if (color) {
        for (int i = 0; i < w * h; i++) {
            ret->data[i] = *color;
        }
    } else {
        PPMPixel black = PPMPixel_create_val(0x00, 0x00, 0x00);
        for (int i = 0; i < w * h; i++) {
            ret->data[i] = black;
        }
    }

    //ret->data2D = NULL;
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
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //read image format
    if (!fgets(buff, sizeof(buff), fp)) {
        perror(filename);
        exit(1);
    }

    //check the image format usign magic bytes
    if (buff[0] != PPM_MAGIC[0] || buff[1] != PPM_MAGIC[1]) {
        fprintf(stderr, "Invalid image format (must be 'P6')\n");
        exit(1);
    }

    //alloc memory form image
    img = (PPMImage *)malloc(sizeof(PPMImage));
    if (!img) {
        fprintf(stderr, "Unable to allocate memory\n");
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
        fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
        exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
        exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color != MAX_CHANNEL_VALUE) {
        fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
        exit(1);
    }

    while (fgetc(fp) != '\n') {
        NOOP;
    }

    //memory allocation for pixel data
    img->data = (PPMPixel *)malloc(img->w * img->h * sizeof(PPMPixel));

    if (!img) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    //read pixel data from file
    if (fread(img->data, sizeof(PPMPixel), img->w * img->h, fp) != img->w * img->h) {
        fprintf(stderr, "Error loading image '%s'\n", filename);
        exit(1);
    }

    fclose(fp);
    return img;
}

void PPMImage_write(const char *filename, PPMImage *img) {
    FILE *fp;

    fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);

        exit(1);
    }

    //write header magic
    fprintf(fp, "P6\n");

    //comments
    fprintf(fp, "# Created by %s\n", CREATOR);

    //image size
    fprintf(fp, "%d %d\n", img->w, img->h);

    // rgb component depth
    fprintf(fp, "%d\n", MAX_CHANNEL_VALUE);

    // pixel data
    if (fwrite(img->data, sizeof(PPMPixel), img->w * img->h, fp) != img->w * img->h) {
        fprintf(stderr, "Something went wrong while writing the data of %s.\n", filename);
    }

    fclose(fp);
}

void PPMImage_draw_pixel(PPMImage *img, int px, int py, PPMColor color) {
    int idx = index2Dto1D(px, py, img->w);
    img->data[idx] = color;
}

void PPMImage_draw_line(PPMImage *image, int x0, int y0, int x1, int y1, PPMColor color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        //setPixel(x0, y0);
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

void PPMImage_draw_rect(PPMImage *image, int x, int y, int w, int h, PPMColor color, int filled) {
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

void PPM_resize_nearest(PPMImage *in, PPMImage *out) {
    if (!in) {
        fprintf(stderr, "PPM_resize_nearest received null image as input.\n");
        exit(1);
    }
    if (!out) {
        fprintf(stderr, "PPM_resize_nearest received null image as output.\n");
        exit(1);
    }

    // TODO: use double when calculating "u" and "v" ?

    for (unsigned int y_out = 0; y_out < out->h; y_out++) {
        const float v = ((float)y_out) / ((float)(out->h));  // v: current position on the output's Y axis (in percentage)
        for (unsigned int x_out = 0; x_out < out->w; x_out++) {
            const float u = ((float)x_out) / ((float)(out->w));  // u: current position on the output's X axis (in percentage)

            const int x_in = (int)(in->w * u);
            const int y_in = (int)(in->h * v);

            out->data[y_out * out->w + x_out] = in->data[y_in * in->w + x_in];
        }
    }
}

PPMImage *PPM_descale_nearest(PPMImage *in, unsigned int assumed_w, unsigned int assumed_h) {
    PPMImage *out = PPMImage_create(assumed_w, assumed_h, NULL);
    double half_pixel_offest = 0.5;
    for (unsigned int y_out = 0; y_out < out->h; y_out++) {
        const double v = ((double)y_out) / ((double)(out->h));  // v: current position on the output's Y axis (in percentage)
        for (unsigned int x_out = 0; x_out < out->w; x_out++) {
            const double u = ((double)x_out) / ((double)(out->w));  // u: current position on the output's X axis (in percentage)

            const int x_in = round(in->w * u + half_pixel_offest);
            const int y_in = round(in->h * v + half_pixel_offest);

            out->data[y_out * out->w + x_out] = in->data[y_in * in->w + x_in];
        }
    }
    return out;
}

/*
PPM_resize_bilinear(PPMImage *in,PPMImage *out,  int out_width, int out_height) {
    out->data = in->data;
}
*/

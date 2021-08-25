#include "ppm.h"

#include <stdio.h>
#include <stdlib.h>

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

    char flag = 0;
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
    if (rgb_comp_color != RGB_COMPONENT_COLOR) {
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
    fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

    // pixel data
    fwrite(img->data, sizeof(PPMPixel), img->w * img->h, fp);
    fclose(fp);
}

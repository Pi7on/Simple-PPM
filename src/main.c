#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "ppm.h"

int index2Dto1D(int row_index, int col_index, int matrix_width) {
    return (matrix_width * row_index) + col_index;
}

void generate_RGB_collage_set() {
    PPMImage *frame = PPMImage_create(256, 256, PPMColor_create_ref(0x000000));
    for (int i = 0; i < 6; i++) {
        for (int r = 0; r < 256; r++) {
            for (int g = 0; g < 256; g++) {
                for (int b = 0; b < 256; b++) {
                    int index = index2Dto1D(g, b, frame->w);
                    switch (i) {
                        case 0:
                            frame->data[index] = PPMPixel_create_val(r, g, b);
                            break;
                        case 1:
                            frame->data[index] = PPMPixel_create_val(r, b, g);
                            break;
                        case 2:
                            frame->data[index] = PPMPixel_create_val(g, b, r);
                            break;
                        case 3:
                            frame->data[index] = PPMPixel_create_val(b, g, r);
                            break;
                        case 4:
                            frame->data[index] = PPMPixel_create_val(b, r, g);
                            break;
                        case 5:
                            frame->data[index] = PPMPixel_create_val(g, r, b);

                            break;
                        default:
                            printf("error: should not be here.\n");
                            exit(1);
                    }
                }
            }
            char fname[128];
            snprintf(fname, sizeof(fname), "..\\data\\frames\\frame%03d.ppm", r);
            printf("Writing frame %d ... ", r);
            PPMImage_write(fname, frame);
            printf("OK.\n");
        }

        //printf("Encoding video ... ");
        //system("ffmpeg -hide_banner -loglevel error -i \"..\\data\\frames\\frame\%03d.ppm\" -vcodec libx264 -crf 0 \"..\\data\\frames\\out.mp4\"");
        //printf("OK.\n");

        printf("Generating collage %d ... ", i);
        char command[256];
        snprintf(command, sizeof(command), "ffmpeg -hide_banner -loglevel error -i \"..\\data\\frames\\frame%%03d.ppm\" -filter_complex \"tile=16x16\" \"..\\data\\frames\\collageRGB_%d.png\"", i);
        system(command);
        printf("OK.\n");

        printf("Deleting frames ... ");
        system("del ..\\data\\frames\\*.ppm");
        printf("OK.\n");
    }
}

typedef struct {
    int x;
    int y;
} V2i;
typedef V2i Point2D;

typedef enum {
    false,
    true
} bool;

/* void PPM_draw_line(PPMImage *canvas, V2i a, V2i b, PPMColor c) {
    for (int canv_y = 0; canv_y < canvas->h; canv_y++) {
        for (int canv_x = 0; canv_x < canvas->h; canv_x++) {
            int draw_p1 = ((canv_x == a.x) && (canv_y == a.y));
            int draw_p2 = ((canv_x == b.x) && (canv_y == b.y));

            if (draw_p1 || draw_p2) {
                int index = index2Dto1D(canv_y, canv_x, canvas->w);
                canvas->data[index] = c;
            }
        }
    }
}
*/

void PPMImage_draw_pixel(PPMImage *img, int px, int py, PPMColor color) {
    for (int rows = 0; rows < img->h; rows++) {
        for (int cols = 0; cols < img->w; cols++) {
            if ((cols == px) && (rows == py)) {
                int idx = index2Dto1D(rows, cols, img->w);
                img->data[idx] = color;
            }
        }
    }
}

void PPMImage_draw_line(PPMImage *image, int x0, int y0, int x1, int y1, PPMColor color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;) {
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

void PPMImage_display(char *playername, char *fname) {
    char *command;
    sprintf(command, "%s %s", playername, fname);
    system(command);
}

int main(void) {
    printf("my firt time using git, so here's a commit test.\n");
    //generate_RGB_collage_set();
    //PPMColor c = PPMColor_create_val(RGB_MAGENTA);
    PPMImage *img = PPMImage_create(256, 256, NULL);

    //PPMImage_draw_line(img, 10, 10, 100, 10, PPMColor_create_val(RGB_YELLOW));
    //PPMImage_draw_line(img, 100, 10, 100, 100, PPMColor_create_val(RGB_MAGENTA));
    //PPMImage_draw_line(img, 100, 100, 10, 10, PPMColor_create_val(RGB_CYAN));

    int bodyx = (img->w / 2) - (img->w / 4);
    int bodyy = 30;
    int bodyw = 30;
    int bodyh = 40;
    PPMImage_draw_rect(img, bodyx, bodyy, bodyw, bodyh, PPMColor_create_val(RGB_RED), 1);

    int backx = bodyx + bodyw;
    int backy = bodyy + 2;
    int backw = 5;
    int backh = bodyh - 15;
    PPMImage_draw_rect(img, backx, backy, backw, backh, PPMColor_create_val(RGB_RED), 1);

    int leglx = bodyx;
    int legly = bodyy + bodyh;
    int leglw = 12;
    int leglh = 8;
    PPMImage_draw_rect(img, leglx, legly, leglw, leglh, PPMColor_create_val(RGB_RED), 1);
    PPMImage_draw_rect(img, leglx + 18, legly, leglw, leglh, PPMColor_create_val(RGB_RED), 1);

    int visx = bodyx + 4;
    int visy = bodyy + 4;
    int visw = bodyw - 15;
    int vish = 7;
    PPMImage_draw_rect(img, visx, visy, visw, vish, PPMColor_create_val(0xEEEEEE), 1);

    PPMImage_write("..\\data\\mag.ppm", img);
    PPMImage_display("C:\\IMAGEGLASS\\ImageGlass.exe", "..\\data\\mag.ppm");
    return 0;
}
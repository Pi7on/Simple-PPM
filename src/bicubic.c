#include "bicubic.h"

#include <stdio.h>
#include <stdlib.h>

#include "ppm.h"

/**
 * Cubic interpolation for a given set of points and x value.
 *
 * @param values An array of 4 pixel intensity values.
 * @param x The position (relative to the four values) to interpolate.
 * @return The interpolated intensity value.
 */
double cubicInterpolate(double values[4], double x) {
  return values[1] + 0.5 * x *
                         (values[2] - values[0] +
                          x * (2.0 * values[0] - 5.0 * values[1] +
                               4.0 * values[2] - values[3] +
                               x * (3.0 * (values[1] - values[2]) + values[3] -
                                    values[0])));
}

PPMPixel bilinearInterpolate(PPMImage* img, double x, double y) {
  int x1 = (int)x;
  int y1 = (int)y;
  int x2 = x1 + 1;
  int y2 = y1 + 1;

  // Clamp the coordinates to ensure they are within bounds
  x1 = (x1 < 0) ? 0 : (x1 >= img->w) ? img->w - 1 : x1;
  y1 = (y1 < 0) ? 0 : (y1 >= img->h) ? img->h - 1 : y1;
  x2 = (x2 < 0) ? 0 : (x2 >= img->w) ? img->w - 1 : x2;
  y2 = (y2 < 0) ? 0 : (y2 >= img->h) ? img->h - 1 : y2;

  // Get four neighboring pixels
  PPMPixel p11 = img->data[y1 * img->w + x1];
  PPMPixel p12 = img->data[y2 * img->w + x1];
  PPMPixel p21 = img->data[y1 * img->w + x2];
  PPMPixel p22 = img->data[y2 * img->w + x2];

  double alpha = x - x1;
  double beta = y - y1;

  unsigned char r = (1 - alpha) * (1 - beta) * p11.chan.r +
                    (1 - alpha) * beta * p12.chan.r +
                    alpha * (1 - beta) * p21.chan.r + alpha * beta * p22.chan.r;

  unsigned char g = (1 - alpha) * (1 - beta) * p11.chan.g +
                    (1 - alpha) * beta * p12.chan.g +
                    alpha * (1 - beta) * p21.chan.g + alpha * beta * p22.chan.g;

  unsigned char b = (1 - alpha) * (1 - beta) * p11.chan.b +
                    (1 - alpha) * beta * p12.chan.b +
                    alpha * (1 - beta) * p21.chan.b + alpha * beta * p22.chan.b;

  PPMPixel result = {.chan = {b, g, r, 0}};
  return result;
}

PPMPixel bicubicInterpolate(PPMImage* img, double posX, double posY) {
  int i, j;
  double interpolatedValues[4];
  PPMPixel surroundingPixels[4][4];

  // Collect surrounding 16 pixels around the position
  for (i = -1; i <= 2; ++i) {
    for (j = -1; j <= 2; ++j) {
      int x = (int)posX + i;
      int y = (int)posY + j;

      // x = (x < 0) ? 0 : (x >= img->w) ? img->w - 1 : x;
      // y = (y < 0) ? 0 : (y >= img->h) ? img->h - 1 : y;

      // Reflect pixels at the borders
      x = (x < 0) ? -x : (x >= img->w) ? 2 * img->w - x - 2 : x;
      y = (y < 0) ? -y : (y >= img->h) ? 2 * img->h - y - 2 : y;

      surroundingPixels[i + 1][j + 1] = img->data[y * img->w + x];
    }
  }

  // Perform cubic interpolation for red, green, and blue channels separately

  // Red channel
  for (i = 0; i < 4; ++i) {
    double redValues[4] = {(double)surroundingPixels[i][0].chan.r,
                           (double)surroundingPixels[i][1].chan.r,
                           (double)surroundingPixels[i][2].chan.r,
                           (double)surroundingPixels[i][3].chan.r};
    interpolatedValues[i] = cubicInterpolate(redValues, posY - (int)posY);
  }
  unsigned char r =
      (unsigned char)cubicInterpolate(interpolatedValues, posX - (int)posX);

  // Green channel
  for (i = 0; i < 4; ++i) {
    double greenValues[4] = {(double)surroundingPixels[i][0].chan.g,
                             (double)surroundingPixels[i][1].chan.g,
                             (double)surroundingPixels[i][2].chan.g,
                             (double)surroundingPixels[i][3].chan.g};
    interpolatedValues[i] = cubicInterpolate(greenValues, posY - (int)posY);
  }
  unsigned char g =
      (unsigned char)cubicInterpolate(interpolatedValues, posX - (int)posX);

  // Blue channel
  for (i = 0; i < 4; ++i) {
    double blueValues[4] = {(double)surroundingPixels[i][0].chan.b,
                            (double)surroundingPixels[i][1].chan.b,
                            (double)surroundingPixels[i][2].chan.b,
                            (double)surroundingPixels[i][3].chan.b};
    interpolatedValues[i] = cubicInterpolate(blueValues, posY - (int)posY);
  }
  unsigned char b =
      (unsigned char)cubicInterpolate(interpolatedValues, posX - (int)posX);

  // Construct the resulting pixel from the interpolated channel values
  PPMPixel result = {.chan = {b, g, r, 0}};
  return result;
}

PPMPixel bicubicInterpolateBuffered(PPMImage* img, double posX, double posY) {
  // Check if pixel is near the borders
  if (posX < 2 || posX > img->w - 3 || posY < 2 || posY > img->h - 3) {
    return bilinearInterpolate(img, posX, posY);
  }
  return bicubicInterpolate(img, posX, posY);
}

/**
 * Resize an image using bicubic interpolation.
 *
 * @param src The source image.
 * @param target_width The desired width.
 * @param target_height The desired height.
 * @return A pointer to the resized image.
 */
PPMImage* resize_bicubic(PPMImage* src, unsigned int target_width,
                         unsigned int target_height) {
  PPMImage* dest = (PPMImage*)malloc(sizeof(PPMImage));
  dest->w = target_width;
  dest->h = target_height;
  dest->data =
      (PPMPixel*)malloc(target_width * target_height * sizeof(PPMPixel));

  double scaleX = (double)src->w / target_width;
  double scaleY = (double)src->h / target_height;

  for (unsigned int x = 0; x < target_width; x++) {
    for (unsigned int y = 0; y < target_height; y++) {
      dest->data[y * target_width + x] =
          bicubicInterpolateBuffered(src, x * scaleX, y * scaleY);
    }
  }

  return dest;
}

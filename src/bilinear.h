#ifndef BILINEAR_H
#define BILINEAR_H

#include "ppm.h"

/**
 * Linearly interpolate two RGB pixels, channel by channel.
 *
 * @param[in] a: First pixel.
 * @param[in] b: Second pixel.
 * @param[in] weight: Weight used for the interpolation of each channel.
 * @param[in] round_flag: If set, rounds result for each color channel.  
 */
PPMPixel PPMPixel_lerp(PPMPixel a, PPMPixel b, const double weight, bool round_flag);

// Resources:
// - https://stackoverflow.com/questions/11991701/image-interpolation-bicubic-or-bilinear-what-if-there-are-no-neighbor-pixels
// - https://tinaja.com/glib/pixintpl.pdf
// - https://www.reddit.com/r/programming/comments/10c4w8/pure_javascript_html5_canvas_bilinear_image/c6ccwwn?utm_source=share&utm_medium=web2x&context=3
/**
 * @param[in] in: Input image.
 * @param[in] w: Target width.
 * @param[in] h: Target height.
 * @param[in] round_flag: Gets passed to PPMPixel_lerp(). If set applies rounding to interpolation results.
*/
PPMImage *PPM_resize_bilinear(PPMImage *in, unsigned int w, unsigned int h, bool round_flag);

#endif /* BILINEAR_H */
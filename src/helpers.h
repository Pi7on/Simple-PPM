#ifndef HELPERS_H
#define HELPERS_H

// Clamp value between min and max
#define CLAMP(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

// linear interpolation
#define LERP(a, b, weight) ((a) == (b) ? (a) : ((a) * (1.0f - (weight)) + ((b) * (weight))))

// Access a 1D array using 2D matrix coordinates and matrix width
#define TWOD_ACCESS(x, y, matrix_width) ((x) + ((y) * (matrix_width)))

#endif /* HELPERS_H */
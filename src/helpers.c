#include "helpers.h"

double lerp_double(const double a, const double b, const double weight) {
    if (a == b) {
        return a;
    }
    return ((a * (1.0f - weight)) + (b * weight));
}
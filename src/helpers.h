#ifndef HELPERS_H
#define HELPERS_H

// Clamp value between min and max
#define CLAMP(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#endif /* HELPERS_H */
#include <cmath>
#include "MathExtensions.h"

real fabsr(real value) {
#ifdef USE_DOUBLE_PRECISION
    return fabs(value);
#else
    return fabsf(value);
#endif
}

real sqrtr(real value) {
#ifdef USE_DOUBLE_PRECISION
    return sqrt(value);
#else
    return sqrtf(value);
#endif
}

real sinr(real value) {
#ifdef USE_DOUBLE_PRECISION
    return sin(value);
#else
    return sinf(value);
#endif
}

real cosr(real value) {
#ifdef USE_DOUBLE_PRECISION
    return cos(value);
#else
    return cosf(value);
#endif
}

real powr(real basis, real exponent) {
#ifdef USE_DOUBLE_PRECISION
    return pow(basis, exponent);
#else
    return powf(basis, exponent);
#endif
}

bool almostEqualsr(real first, real second, real epsilon) {
    return fabsr(first - second) <= epsilon;
}

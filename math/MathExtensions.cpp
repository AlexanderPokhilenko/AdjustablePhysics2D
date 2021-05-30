#include <cmath>
#include "MathExtensions.h"

real fabsr(real value) {
#ifdef DOUBLE_PRECISION
    return fabs(value);
#else
    return fabsf(value);
#endif
}

bool almostEquals(real first, real second, real epsilon) {
    return fabsr(first - second) <= epsilon;
}

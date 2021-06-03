#ifndef ADJUSTABLEPHYSICS2D_MATHEXTENSIONS_H
#define ADJUSTABLEPHYSICS2D_MATHEXTENSIONS_H

#include "../base/primitives.h"
#include <cfloat>

static constexpr const real Epsilon = 1e-3;
#ifdef USE_DOUBLE_PRECISION
#define REAL_MAX DBL_MAX
#define REAL_MIN DBL_MIN
#else
#define REAL_MAX FLT_MAX
#define REAL_MIN FLT_MIN
#endif

real fabsr(real value);
real sqrtr(real value);
real sinr(real value);
real cosr(real value);
bool almostEqualsr(real first, real second, real epsilon = Epsilon);

#endif //ADJUSTABLEPHYSICS2D_MATHEXTENSIONS_H

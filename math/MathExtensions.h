#ifndef PHYSICSGUI1_MATHEXTENSIONS_H
#define PHYSICSGUI1_MATHEXTENSIONS_H

#include "../base/primitives.h"
#include <cfloat>

static constexpr const real Epsilon = 1e-3;
#ifdef DOUBLE_PRECISION
#define REAL_MAX DBL_MAX
#define REAL_MIN DBL_MIN
#else
#define REAL_MAX FLT_MAX
#define REAL_MIN FLT_MIN
#endif
//TODO: add more functions
real fabsr(real value);
bool almostEquals(real first, real second, real epsilon = Epsilon);

#endif //PHYSICSGUI1_MATHEXTENSIONS_H

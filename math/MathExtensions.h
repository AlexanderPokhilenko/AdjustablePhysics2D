#ifndef PHYSICSGUI1_MATHEXTENSIONS_H
#define PHYSICSGUI1_MATHEXTENSIONS_H

#include "../base/primitives.h"

static constexpr const real Epsilon = 1e-3;
//TODO: add more functions
real fabsr(real value);
bool almostEquals(real first, real second, real epsilon = Epsilon);

#endif //PHYSICSGUI1_MATHEXTENSIONS_H

#ifndef ADJUSTABLEPHYSICS2D_PRIMITIVES_H
#define ADJUSTABLEPHYSICS2D_PRIMITIVES_H

#include <cstddef>

#ifdef USE_DOUBLE_PRECISION
using real = double;
#else
using real = float;
#endif

using EntityId = size_t;

#endif //ADJUSTABLEPHYSICS2D_PRIMITIVES_H

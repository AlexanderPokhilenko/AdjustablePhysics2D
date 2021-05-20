#ifndef ADJUSTABLEPHYSICS2D_TRANSFORM_H
#define ADJUSTABLEPHYSICS2D_TRANSFORM_H

#include "../math/Vector2.h"

struct Transform {
    Vector2 linear;
#ifdef USE_ROTATION
    real angular;
#endif
};

#endif //ADJUSTABLEPHYSICS2D_TRANSFORM_H

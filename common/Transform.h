#ifndef ADJUSTABLEPHYSICS2D_TRANSFORM_H
#define ADJUSTABLEPHYSICS2D_TRANSFORM_H

#include "../math/Vector2.h"

struct Transform {
    Vector2 position;
#ifdef USE_ROTATION
    real rotation;
#endif
};

#endif //ADJUSTABLEPHYSICS2D_TRANSFORM_H

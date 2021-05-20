#ifndef ADJUSTABLEPHYSICS2D_SHAPE_H
#define ADJUSTABLEPHYSICS2D_SHAPE_H

#include "../math/Vector2.h"
#include "AABB.h"

struct Shape {
    Vector2 centroid;
    AABB boundingBox;
    real radius;
};

#endif //ADJUSTABLEPHYSICS2D_SHAPE_H

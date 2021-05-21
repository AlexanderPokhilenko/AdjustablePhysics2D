#ifndef ADJUSTABLEPHYSICS2D_TRANSFORM_H
#define ADJUSTABLEPHYSICS2D_TRANSFORM_H

#include "../math/Vector2.h"

struct Transform {
    Vector2 linear;
#ifdef USE_ROTATION
    real angular;
#endif

    Transform operator -() const;
    Transform operator +(const Transform& second) const;
    Transform operator -(const Transform& second) const;
    Transform& operator +=(const Transform& right);
    Transform& operator -=(const Transform& right);
    Transform operator *(real k) const;
    Transform operator /(real k) const;
};

#endif //ADJUSTABLEPHYSICS2D_TRANSFORM_H

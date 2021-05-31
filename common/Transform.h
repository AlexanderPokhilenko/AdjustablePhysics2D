#ifndef ADJUSTABLEPHYSICS2D_TRANSFORM_H
#define ADJUSTABLEPHYSICS2D_TRANSFORM_H

#include "../math/Vector2.h"

struct Transform {
    Vector2 linear;
    Transform();
#ifdef USE_ROTATION
    real angular;
    Transform(Vector2 linear, real angular);
#endif
    explicit Transform(Vector2 linear);

    Transform operator -() const;
    Transform operator +(const Transform& second) const;
    Transform operator -(const Transform& second) const;
    Transform& operator +=(const Transform& right);
    Transform& operator -=(const Transform& right);
    Transform operator *(real k) const;
    Transform operator /(real k) const;
};

#endif //ADJUSTABLEPHYSICS2D_TRANSFORM_H

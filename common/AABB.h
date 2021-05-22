#ifndef ADJUSTABLEPHYSICS2D_AABB_H
#define ADJUSTABLEPHYSICS2D_AABB_H

#include "../math/Vector2.h"

struct AABB {
    Vector2 min;
    Vector2 max;

    Vector2 getCenter() const;

    AABB operator +(const Vector2& vector) const;
    AABB operator -(const Vector2& vector) const;
    AABB& operator +=(const Vector2& vector);
    AABB& operator -=(const Vector2& vector);
};

#endif //ADJUSTABLEPHYSICS2D_AABB_H

#ifndef ADJUSTABLEPHYSICS2D_VECTOR2_H
#define ADJUSTABLEPHYSICS2D_VECTOR2_H

#include "../base/primitives.h"

struct Vector2 {
    real x;
    real y;

    static real dotProduct(Vector2 first, Vector2 second);
    static real crossProduct(Vector2 first, Vector2 second);

    real getMagnitude() const;
    real getSqrMagnitude() const;
    Vector2 getNormalized() const;
    void normalize();
    Vector2 getRotated(real angle) const;
    void rotate(real angle);

    Vector2 operator -() const;
    Vector2 operator +(const Vector2& second) const;
    Vector2 operator -(const Vector2& second) const;
    Vector2& operator +=(const Vector2& right);
    Vector2& operator -=(const Vector2& right);
    Vector2 operator *(real k) const;
    Vector2 operator /(real k) const;
};

#endif //ADJUSTABLEPHYSICS2D_VECTOR2_H

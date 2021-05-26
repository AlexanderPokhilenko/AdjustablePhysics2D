#ifndef ADJUSTABLEPHYSICS2D_VECTOR2_H
#define ADJUSTABLEPHYSICS2D_VECTOR2_H

#include "../base/primitives.h"

struct Vector2 {
    real x;
    real y;

    static bool almostEquals(const Vector2& first, const Vector2& second, real epsilon = 1e-3);

    static real dotProduct(const Vector2& first, const Vector2& second);
    static real crossProduct(const Vector2& first, const Vector2& second);

    real getMagnitude() const;
    real getSqrMagnitude() const;
    Vector2 getNormalized() const;
    void normalize();
    Vector2 getRotated(real angle) const;
    void rotate(real angle);

    bool operator ==(const Vector2& right) const;
    Vector2 operator -() const;
    Vector2 operator +(const Vector2& second) const;
    Vector2 operator -(const Vector2& second) const;
    Vector2& operator +=(const Vector2& right);
    Vector2& operator -=(const Vector2& right);
    Vector2 operator *(real k) const;
    Vector2 operator /(real k) const;
};

#endif //ADJUSTABLEPHYSICS2D_VECTOR2_H

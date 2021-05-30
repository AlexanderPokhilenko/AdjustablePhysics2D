#include "AABB.h"

Vector2 AABB::getCenter() const {
    return (max - min) / 2;
}

Vector2 AABB::getSize() const {
    return {
        max.x - min.x,
        max.y - min.y
    };
}

Vector2 AABB::getHalfSize() const {
    return {
            (max.x - min.x) * real(0.5),
            (max.y - min.y) * real(0.5)
    };
}

real AABB::getArea() const {
    auto size = getSize();
    return size.x * size.y;
}

#ifdef USE_INERTIA
real AABB::getInertia(real mass) const {
    return mass * getSize().getSqrMagnitude() / real(12);
}
#endif

AABB AABB::operator+(const Vector2 &vector) const {
    return {min + vector, max + vector};
}

AABB AABB::operator-(const Vector2 &vector) const {
    return {min - vector, max - vector};
}

AABB &AABB::operator+=(const Vector2 &vector) {
    min += vector;
    max += vector;
    return *this;
}

AABB &AABB::operator-=(const Vector2 &vector) {
    min -= vector;
    max -= vector;
    return *this;
}

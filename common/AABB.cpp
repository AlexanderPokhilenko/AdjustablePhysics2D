#include "AABB.h"

Vector2 AABB::getCenter() const {
    return (max - min) / 2;
}

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

#include "Shape.h"

Vector2 Shape::getCenter() const {
#ifndef USE_AABB_ONLY
    return centroid;
#elif !defined(USE_CIRCLES_ONLY)
    return boundingBox.getCenter();
#else
#error Unknown shape format!
#endif
}

Shape Shape::operator+(const Vector2 &vector) const {
    return {
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
            shapeType,
#endif
#ifndef USE_CIRCLES_ONLY
            boundingBox + vector,
#endif
#ifndef USE_AABB_ONLY
            centroid + vector,
            radius
#endif
    };
}

Shape Shape::operator-(const Vector2 &vector) const {
    return {
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
            shapeType,
#endif
#ifndef USE_CIRCLES_ONLY
            boundingBox - vector,
#endif
#ifndef USE_AABB_ONLY
            centroid - vector,
            radius
#endif
    };
}

Shape &Shape::operator+=(const Vector2 &vector) {
#ifndef USE_CIRCLES_ONLY
    boundingBox += vector;
#endif
#ifndef USE_AABB_ONLY
    centroid += vector;
#endif
    return *this;
}

Shape &Shape::operator-=(const Vector2 &vector) {
#ifndef USE_CIRCLES_ONLY
    boundingBox -= vector;
#endif
#ifndef USE_AABB_ONLY
    centroid -= vector;
#endif
    return *this;
}

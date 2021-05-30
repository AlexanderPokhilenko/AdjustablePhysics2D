#ifndef ADJUSTABLEPHYSICS2D_SHAPE_H
#define ADJUSTABLEPHYSICS2D_SHAPE_H

#include "../math/Vector2.h"
#include "AABB.h"
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
#include "ShapeType.h"
#endif

struct Shape {
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
    ShapeType shapeType;
#endif
#ifndef USE_CIRCLES_ONLY
    AABB boundingBox;
#endif
#ifndef USE_AABB_ONLY
    Vector2 centroid;
    real radius;
#endif

    Vector2 getCenter() const;
    bool tryGetArea(real &area) const;
#ifdef USE_INERTIA
    bool tryGetInertia(real mass, real &inertia) const;
#endif

    Shape operator +(const Vector2& vector) const;
    Shape operator -(const Vector2& vector) const;
    Shape& operator +=(const Vector2& vector);
    Shape& operator -=(const Vector2& vector);
};

#endif //ADJUSTABLEPHYSICS2D_SHAPE_H

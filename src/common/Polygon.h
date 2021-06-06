#ifndef ADJUSTABLEPHYSICS2D_POLYGON_H
#define ADJUSTABLEPHYSICS2D_POLYGON_H

#ifndef USE_CIRCLES_ONLY
#include "AABB.h"
#endif
#include "../math/Vector2.h"

struct Polygon
{
    size_t count;
    Vector2* vertices;
    Vector2* normals;
    Polygon();
    Polygon(size_t count, Vector2 vertices[], Vector2 normals[]);
    real getArea() const;
#ifdef USE_INERTIA
    real getInertia(real mass) const;
#endif
#ifndef USE_CIRCLES_ONLY
    AABB getAABB(real angle = 0) const;
#endif
#ifndef USE_AABB_ONLY
    real getRadius() const;
#endif
    ~Polygon();
};

#endif //ADJUSTABLEPHYSICS2D_POLYGON_H

#include "Polygon.h"
#include <cmath>
#include <cfloat>

#ifndef USE_CIRCLES_ONLY
AABB Polygon::getAABB(real angle) const {
#ifdef DOUBLE_PRECISION
    Vector2 min = {DBL_MAX, DBL_MAX};
#else
    Vector2 min = {FLT_MAX, FLT_MAX};
#endif
    Vector2 max = {0, 0};

    for (int i = 0; i < count; ++i) {
        auto edge = edges[i].getRotated(angle);
        if(edge.x < min.x) min.x = edge.x;
        if(edge.x > max.x) max.x = edge.x;
        if(edge.y < min.y) min.y = edge.y;
        if(edge.y > max.y) max.y = edge.y;
    }

    return {min, max};
}
#endif

#ifndef USE_AABB_ONLY
real Polygon::getRadius() const {
    real maxSqr = 0;

    for (int i = 0; i < count; ++i) {
        auto current = edges[i].getSqrMagnitude();
        if(current > maxSqr) maxSqr = current;
    }

#ifdef DOUBLE_PRECISION
    return sqrt(maxSqr);
#else
    return sqrtf(maxSqr);
#endif
}
#endif

Polygon::~Polygon()
{
    delete[] edges;
    delete[] normals;
}

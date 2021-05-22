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
        auto side = sides[i].getRotated(angle);
        if(side.x < min.x) min.x = side.x;
        if(side.x > max.x) max.x = side.x;
        if(side.y < min.y) min.y = side.y;
        if(side.y > max.y) max.y = side.y;
    }

    return {min, max};
}
#endif

#ifndef USE_AABB_ONLY
real Polygon::getRadius() const {
    real maxSqr = 0;

    for (int i = 0; i < count; ++i) {
        auto current = sides[i].getSqrMagnitude();
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
    delete[] sides;
    delete[] normals;
}

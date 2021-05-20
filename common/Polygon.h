#ifndef ADJUSTABLEPHYSICS2D_POLYGON_H
#define ADJUSTABLEPHYSICS2D_POLYGON_H

#include "../math/Vector2.h"

struct Polygon
{
    int count;
    Vector2* sides;
    Vector2* normals;
    ~Polygon()
    {
        delete[] sides;
        delete[] normals;
    };
};

#endif //ADJUSTABLEPHYSICS2D_POLYGON_H

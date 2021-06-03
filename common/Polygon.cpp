#include "Polygon.h"
#include "../math/MathExtensions.h"
#include <cfloat>

Polygon::Polygon() : count(0), vertices(nullptr), normals(nullptr)
{ }

Polygon::Polygon(size_t count, Vector2 *vertices, Vector2 *normals) : count(count), vertices(vertices), normals(normals)
{ }

real Polygon::getArea() const {
    auto lastIndex = count - 1;
    real area = Vector2::crossProduct(vertices[lastIndex], vertices[0]);
    for (size_t i = 0; i < lastIndex; ++i) {
        auto first = vertices[i], second = vertices[i + 1];
        area += Vector2::crossProduct(first, second);
    }
    area *= real(0.5);
    return std::abs(area);
}

#ifdef USE_INERTIA
real Polygon::getInertia(real mass) const {
    auto lastIndex = count - 1;
    auto first = vertices[lastIndex], second = vertices[0];
    auto crossProduct = Vector2::crossProduct(first, second);
    real area = crossProduct;
    real inertia = crossProduct * (Vector2::dotProduct(first, first) + Vector2::dotProduct(first, second) + Vector2::dotProduct(second, second));
    for (size_t i = 0; i < lastIndex; ++i) {
        first = vertices[i], second = vertices[i + 1];
        crossProduct = Vector2::crossProduct(first, second);
        area += crossProduct;
        inertia += crossProduct * (Vector2::dotProduct(first, first) + Vector2::dotProduct(first, second) + Vector2::dotProduct(second, second));
    }
    area *= real(0.5);
    inertia /= area;
    inertia *= mass / real(12);
    return std::abs(inertia);
}
#endif

#ifndef USE_CIRCLES_ONLY
AABB Polygon::getAABB(real angle) const {
    Vector2 min = {REAL_MAX, REAL_MAX};
    Vector2 max = {0, 0};

    for (size_t i = 0; i < count; ++i) {
        auto edge = vertices[i].getRotated(angle);
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

    for (size_t i = 0; i < count; ++i) {
        auto current = vertices[i].getSqrMagnitude();
        if(current > maxSqr) maxSqr = current;
    }

    return sqrtr(maxSqr);
}
#endif

Polygon::~Polygon()
{
    delete[] vertices;
    delete[] normals;
}

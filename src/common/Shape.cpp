#include "Shape.h"
#include "../PhysicsEngine.h"
#include <cmath>

Vector2 Shape::getCenter() const {
#ifndef USE_AABB_ONLY
    return centroid;
#elif !defined(USE_CIRCLES_ONLY)
    return boundingBox.getCenter();
#else
#error Unknown shape format!
#endif
}

bool Shape::tryGetArea(real &area) const {
#ifdef USE_CIRCLES_ONLY
    area = real(M_PI) * radius * radius;
    return true;
#elif defined(USE_AABB_ONLY)
    area = boundingBox.getArea();
    return true;
#else
    switch (shapeType) {
        case ShapeType::Circle:
            area = real(M_PI) * radius * radius;
            return true;
        case ShapeType::AABB:
            area = boundingBox.getArea();
            return true;
        default:
            return false;
    }
#endif
}

#ifdef USE_INERTIA
bool Shape::tryGetInertia(real mass, real &inertia) const {
#ifdef USE_CIRCLES_ONLY
    inertia = PhysicsEngine::DefaultInertiaCoefficient * mass * radius * radius;
    return true;
#elif defined(USE_AABB_ONLY)
    inertia = boundingBox.getInertia(mass);
    return true;
#else
    switch (shapeType) {
        case ShapeType::Circle:
            inertia = PhysicsEngine::DefaultInertiaCoefficient * mass * radius * radius;
            return true;
        case ShapeType::AABB:
            inertia = boundingBox.getInertia(mass);
            return true;
        default:
            return false;
    }
#endif
}
#endif

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

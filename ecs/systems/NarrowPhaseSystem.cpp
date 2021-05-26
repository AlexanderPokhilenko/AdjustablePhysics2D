#include "NarrowPhaseSystem.h"
#include <cmath>
#include <cfloat>

ComponentsBitset createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Shape));
    return bitset;
}

NarrowPhaseSystem::NarrowPhaseSystem() : System(createCurrentSystemBitset()), handlers() {
#ifndef USE_PRIMITIVES_ONLY
    handlers[static_cast<size_t>(ShapeType::Complex)][static_cast<size_t>(ShapeType::Complex)] = &NarrowPhaseSystem::Convex2Convex;
#endif
#ifndef USE_CIRCLES_ONLY
    handlers[static_cast<size_t>(ShapeType::AABB)][static_cast<size_t>(ShapeType::AABB)] = &NarrowPhaseSystem::AABB2AABB;
#ifndef USE_AABB_ONLY
    handlers[static_cast<size_t>(ShapeType::AABB)][static_cast<size_t>(ShapeType::Circle)] = &NarrowPhaseSystem::AABB2Circle;
    handlers[static_cast<size_t>(ShapeType::Circle)][static_cast<size_t>(ShapeType::AABB)] = &NarrowPhaseSystem::Circle2AABB;
#endif
#ifndef USE_PRIMITIVES_ONLY
    handlers[static_cast<size_t>(ShapeType::Complex)][static_cast<size_t>(ShapeType::AABB)] = &NarrowPhaseSystem::Convex2AABB;
    handlers[static_cast<size_t>(ShapeType::AABB)][static_cast<size_t>(ShapeType::Complex)] = &NarrowPhaseSystem::AABB2Convex;
#endif
#endif
#ifndef USE_AABB_ONLY
    handlers[static_cast<size_t>(ShapeType::Circle)][static_cast<size_t>(ShapeType::Circle)] = &NarrowPhaseSystem::Circle2Circle;
#ifndef USE_PRIMITIVES_ONLY
    handlers[static_cast<size_t>(ShapeType::Complex)][static_cast<size_t>(ShapeType::Circle)] = &NarrowPhaseSystem::Convex2Circle;
    handlers[static_cast<size_t>(ShapeType::Circle)][static_cast<size_t>(ShapeType::Complex)] = &NarrowPhaseSystem::Circle2Convex;
#endif
#endif
}

void NarrowPhaseSystem::update(Context &context, EntityId id1, real deltaTime) {
#ifndef USE_BROAD_PHASE
    auto entitiesSize = context.getEntitiesSize();
    for (size_t id2 = id1 + 1; id2 < entitiesSize; ++id2) {
        if(!context.checkEntity(id2, componentsBitset)) continue;
        handle(context, id1, id2, deltaTime);
    }
#endif
}

void NarrowPhaseSystem::update(Context &context, real deltaTime) {
    context.collisions.clear();
#ifdef USE_BROAD_PHASE
#ifdef USE_CIRCLES_ONLY
    for (auto pair : context.possibleCollisions) Circle2Circle(context, pair.first, pair.second, deltaTime);
#elif defined(USE_AABB_ONLY)
    for (auto pair : context.possibleCollisions) AABB2AABB(context, pair.first, pair.second, deltaTime);
#else
    for (auto pair : context.possibleCollisions) handle(context, pair.first, pair.second, deltaTime);
#endif
#else
    System::update(context, deltaTime);
#endif
}

void NarrowPhaseSystem::handle(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &shape1 = context.getComponent<ShapeComponent>(id1);
    auto &shape2 = context.getComponent<ShapeComponent>(id2);
    handler_function handlerFunc = handlers[static_cast<size_t>(shape1.shapeType)][static_cast<size_t>(shape2.shapeType)];
    (*handlerFunc)(context, id1, id2, deltaTime);
}

#ifndef USE_PRIMITIVES_ONLY
bool NarrowPhaseSystem::checkPenetration(real min1, real max1, real min2, real max2, const Vector2 &axis, Vector2& normal, real &penetration) {
    if (min1 < min2) {
        auto depth = max1 - min2;
        if (depth < 0) return false;
        if (depth < penetration) {
            normal = -axis;
            penetration = depth;
        }
    } else {
        auto depth = max2 - min1;
        if (depth < 0) return false;
        if (depth < penetration) {
            normal = axis;
            penetration = depth;
        }
    }
    return true;
}
#ifdef USE_ROTATION
void NarrowPhaseSystem::getGlobalVectors(const Polygon& polygon, const Vector2& position, real angle, Vector2* edges, Vector2* normals) {
#else
void NarrowPhaseSystem::getGlobalVectors(const Polygon& polygon, const Vector2& position, Vector2* edges, Vector2* normals) {
#endif
    auto size = polygon.count;
    auto localEdges = polygon.edges;
    auto localNormals = polygon.normals;
    for (size_t i = 0; i < size; ++i) {
#ifdef USE_ROTATION
        edges[i] = localEdges[i].getRotated(angle) + position;
        normals[i] = localNormals[i].getRotated(angle) + position;
#else
        edges[i] = localEdges[i] + position;
        normals[i] = localNormals[i] + position;
#endif
    }
}

void NarrowPhaseSystem::getMinMaxOnAxis(const Vector2 *edges, size_t count, const Vector2 &axis, real &min, real &max) {
#ifdef DOUBLE_PRECISION
    min = DBL_MAX;
    max = -DBL_MAX;
#else
    min = FLT_MAX;
    max = -FLT_MAX;
#endif
    for (size_t i = 0; i < count; ++i) {
        auto value = Vector2::dotProduct(edges[i], axis);
        if(value < min) min = value;
        if(value > max) max = value;
    }
}

void NarrowPhaseSystem::Convex2Convex(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &polygon1 = context.getComponent<PolygonComponent>(id1);
    auto &polygon2 = context.getComponent<PolygonComponent>(id2);
#ifdef USE_ROTATION
    auto &location1 = context.getComponent<LocationComponent>(id1);
    auto angle1 = location1.angular;
    auto position1 = location1.linear;

    auto &location2 = context.getComponent<LocationComponent>(id2);
    auto angle2 = location2.angular;
    auto position2 = location2.linear;
#else
    auto position1 = context.getComponent<ShapeComponent>(id1).getCenter();
    auto position2 = context.getComponent<ShapeComponent>(id2).getCenter();
#endif
    auto size1 = polygon1.count;
    Vector2 edges1[size1];
    Vector2 normals1[size1];

    auto size2 = polygon2.count;
    Vector2 edges2[size2];
    Vector2 normals2[size2];
#ifdef USE_ROTATION
    getGlobalVectors(polygon1, position1, angle1, edges1, normals1);
    getGlobalVectors(polygon2, position2, angle2, edges2, normals2);
#else
    getGlobalVectors(polygon1, position1, edges1, normals1);
    getGlobalVectors(polygon2, position2, edges2, normals2);
#endif
#ifdef DOUBLE_PRECISION
    real minDepth = DBL_MAX;
#else
    real minDepth = FLT_MAX;
#endif
    real min1, max1, min2, max2;
    Vector2 axis{}, penetration{};

    for (size_t i = 0; i < size1; ++i) {
        axis = normals1[i];
        getMinMaxOnAxis(edges1, size1, axis, min1, max1);
        getMinMaxOnAxis(edges2, size2, axis, min2, max2);
        if(!checkPenetration(min1, max1, min2, max2, axis, penetration, minDepth)) return;
    }

    for (size_t i = 0; i < size2; ++i) {
        axis = normals2[i];
        getMinMaxOnAxis(edges1, size1, axis, min1, max1);
        getMinMaxOnAxis(edges2, size2, axis, min2, max2);
        if(!checkPenetration(min1, max1, min2, max2, axis, penetration, minDepth)) return;
    }

    Collision collision {id1, id2, minDepth, penetration};
    context.collisions.push_back(collision);
}
#endif
#ifndef USE_CIRCLES_ONLY
void NarrowPhaseSystem::AABB2AABB(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &shape1 = context.getComponent<ShapeComponent>(id1);
    auto &shape2 = context.getComponent<ShapeComponent>(id2);
    auto position1 = shape1.getCenter();
    auto position2 = shape2.getCenter();
    auto halfSize1 = shape1.boundingBox.getHalfSize();
    auto halfSize2 = shape2.boundingBox.getHalfSize();
    auto halfSum = halfSize1 + halfSize2;

    auto displacement = position2 - position1;
    Vector2 penetration = {
            displacement.x >= 0 ? halfSum.x - displacement.x : halfSum.x + displacement.x,
            displacement.y >= 0 ? halfSum.y - displacement.y : halfSum.y + displacement.y
    };

    if(penetration.x <= 0 || penetration.y <= 0) return;

    if(penetration.x <= penetration.y) {
        Collision collision {id1, id2, penetration.x, {real(displacement.x < 0 ? - 1 : 1), 0}};
        context.collisions.push_back(collision);
    } else {
        Collision collision {id1, id2, penetration.y, {0, real(displacement.y < 0 ? -1 : 1)}};
        context.collisions.push_back(collision);
    }
}
#ifndef USE_AABB_ONLY
void NarrowPhaseSystem::Circle2AABB(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    AABB2Circle(context, id2, id1, deltaTime);
}

void NarrowPhaseSystem::AABB2Circle(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &shape1 = context.getComponent<ShapeComponent>(id1);
    auto aabb1 = shape1.boundingBox;
    auto &shape2 = context.getComponent<ShapeComponent>(id2);
    auto center2 = shape2.centroid;
    auto radius2 = shape2.radius;

    auto displacement = center2 - shape1.getCenter();
    auto halfSize = aabb1.getHalfSize();
    Vector2 closest = {
            displacement.x >= halfSize.x ? halfSize.x : displacement.x <= -halfSize.x ? -halfSize.x : displacement.x,
            displacement.y >= halfSize.y ? halfSize.y : displacement.y <= -halfSize.y ? -halfSize.y : displacement.y
    };

    bool inside = false;
    if(closest == displacement) {
        inside = true;
        if(std::abs(displacement.x) > std::abs(displacement.y)) {
            closest.x = closest.x > 0 ? halfSize.x : -halfSize.x;
        } else {
            closest.y = closest.y > 0 ? halfSize.y : - halfSize.y;
        }
    }

    auto normal = displacement - closest;
    auto sqrDistance = normal.getSqrMagnitude();
    if(sqrDistance > radius2 * radius2 && !inside) return;
#ifdef DOUBLE_PRECISION
    auto distance = sqrt(sqrDistance);
#else
    auto penetration = radius2 - sqrtf(sqrDistance);
#endif
    Collision collision {id1, id2, penetration, (inside ? -normal : normal).getNormalized()};
    context.collisions.push_back(collision);
}
#endif
#ifndef USE_PRIMITIVES_ONLY
void NarrowPhaseSystem::AABB2Convex(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    Convex2AABB(context, id2, id1, deltaTime);
}

void NarrowPhaseSystem::Convex2AABB(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &polygon1 = context.getComponent<PolygonComponent>(id1);
    auto &aabb2 = context.getComponent<ShapeComponent>(id2).boundingBox;
    auto size = polygon1.count;
    auto totalSize = size + 2;
#ifdef USE_ROTATION
    auto &location1 = context.getComponent<LocationComponent>(id1);
    auto angle = location1.angular;
    auto position1 = location1.linear;
#else
    auto position1 = context.getComponent<ShapeComponent>(id1).getCenter();
#endif
    Vector2 edges1[size];
    Vector2 edges2[4] = {aabb2.max, {aabb2.min.x, aabb2.max.y}, aabb2.min, {aabb2.max.x, aabb2.min.y}};
    Vector2 normals[totalSize];
#ifdef USE_ROTATION
    getGlobalVectors(polygon1, position1, angle, edges1, normals);
#else
    getGlobalVectors(polygon1, position1, edges, normals);
#endif
    normals[size] = {1, 0};
    normals[size+1] = {0, 1};
#ifdef DOUBLE_PRECISION
    real minDepth = DBL_MAX;
#else
    real minDepth = FLT_MAX;
#endif

    real min1, max1, min2, max2;
    Vector2 axis{}, penetration{};
    for (size_t i = 0; i < totalSize; ++i) {
        axis = normals[i];
        getMinMaxOnAxis(edges1, size, axis, min1, max1);
        getMinMaxOnAxis(edges2, 4, axis, min2, max2);
        if(!checkPenetration(min1, max1, min2, max2, axis, penetration, minDepth)) return;
    }

    Collision collision {id1, id2, minDepth, penetration};
    context.collisions.push_back(collision);
}
#endif
#endif
#ifndef USE_AABB_ONLY
void NarrowPhaseSystem::Circle2Circle(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &shape1 = context.getComponent<ShapeComponent>(id1);
    auto &shape2 = context.getComponent<ShapeComponent>(id2);

    auto sumR = shape1.radius + shape2.radius;
    auto displacement = shape2.centroid - shape1.centroid;

    if(sumR * sumR >= displacement.getSqrMagnitude()) return;

    auto penetration = displacement.getMagnitude() - sumR;
    Collision collision {id1, id2, penetration, displacement.getNormalized()};
    context.collisions.push_back(collision);
}
#ifndef USE_PRIMITIVES_ONLY
void NarrowPhaseSystem::getMinMaxOnAxis(const Vector2 &center, real radius, const Vector2 &axis, real &min, real &max) {
    auto circleProjection = Vector2::dotProduct(center, axis);
    min = circleProjection - radius;
    max = circleProjection + radius;
}

void NarrowPhaseSystem::Circle2Convex(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    Convex2Circle(context, id2, id1, deltaTime);
}

void NarrowPhaseSystem::Convex2Circle(Context &context, EntityId id1, EntityId id2, real deltaTime) {
    auto &polygon1 = context.getComponent<PolygonComponent>(id1);
    auto &shape2 = context.getComponent<ShapeComponent>(id2);
    auto center2 = shape2.centroid;
    auto radius2 = shape2.radius;
    auto size = polygon1.count;
#ifdef USE_ROTATION
    auto &location1 = context.getComponent<LocationComponent>(id1);
    auto angle = location1.angular;
    auto position1 = location1.linear;
#else
    auto position1 = context.getComponent<ShapeComponent>(id1).getCenter();
#endif
    Vector2 edges[size];
    Vector2 normals[size];
#ifdef USE_ROTATION
    getGlobalVectors(polygon1, position1, angle, edges, normals);
#else
    getGlobalVectors(polygon1, position1, edges, normals);
#endif
#ifdef DOUBLE_PRECISION
    real minDepth = DBL_MAX;
#else
    real minDepth = FLT_MAX;
#endif
    real min1, max1, min2, max2;
    Vector2 axis{}, penetration{};
    for (size_t i = 0; i < size; ++i) {
        axis = normals[i];
        getMinMaxOnAxis(edges, size, axis, min1, max1);
        getMinMaxOnAxis(center2, radius2, axis, min2, max2);
        if(!checkPenetration(min1, max1, min2, max2, axis, penetration, minDepth)) return;
    }

    Collision collision {id1, id2, minDepth, penetration};
    context.collisions.push_back(collision);
}
#endif
#endif

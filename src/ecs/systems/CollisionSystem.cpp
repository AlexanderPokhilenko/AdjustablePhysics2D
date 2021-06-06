#include "CollisionSystem.h"
#include "NarrowPhaseSystem.h"
#include <cfloat>
#include "../../math/MathExtensions.h"

#if defined(USE_COLLISION_HOOKE) || defined(USE_COLLISION_IMPULSE)
#define NEED_VELOCITY_CHECK
#endif

CollisionSystem::CollisionSystem() : System()
#ifdef USE_COLLISION_HOOKE
, k(1000)
#elif defined(USE_COLLISION_IMPULSE)
, handlers()
#endif
{
#ifdef USE_COLLISION_IMPULSE
#ifndef USE_PRIMITIVES_ONLY
    handlers[static_cast<size_t>(ShapeType::Complex)][static_cast<size_t>(ShapeType::Complex)] = &CollisionSystem::Convex2Convex;
#endif
#ifndef USE_CIRCLES_ONLY
    handlers[static_cast<size_t>(ShapeType::AABB)][static_cast<size_t>(ShapeType::AABB)] = &CollisionSystem::AABB2AABB;
#ifndef USE_AABB_ONLY
    handlers[static_cast<size_t>(ShapeType::AABB)][static_cast<size_t>(ShapeType::Circle)] = &CollisionSystem::AABB2Circle;
    handlers[static_cast<size_t>(ShapeType::Circle)][static_cast<size_t>(ShapeType::AABB)] = &CollisionSystem::Circle2AABB;
#endif
#ifndef USE_PRIMITIVES_ONLY
    handlers[static_cast<size_t>(ShapeType::Complex)][static_cast<size_t>(ShapeType::AABB)] = &CollisionSystem::Convex2AABB;
    handlers[static_cast<size_t>(ShapeType::AABB)][static_cast<size_t>(ShapeType::Complex)] = &CollisionSystem::AABB2Convex;
#endif
#endif
#ifndef USE_AABB_ONLY
    handlers[static_cast<size_t>(ShapeType::Circle)][static_cast<size_t>(ShapeType::Circle)] = &CollisionSystem::Circle2Circle;
#ifndef USE_PRIMITIVES_ONLY
    handlers[static_cast<size_t>(ShapeType::Complex)][static_cast<size_t>(ShapeType::Circle)] = &CollisionSystem::Convex2Circle;
    handlers[static_cast<size_t>(ShapeType::Circle)][static_cast<size_t>(ShapeType::Complex)] = &CollisionSystem::Circle2Convex;
#endif
#endif
#endif
}

#ifdef USE_COLLISION_IMPULSE
#ifndef USE_PRIMITIVES_ONLY
void CollisionSystem::getVerticesWithMaxProjection(const PolygonComponent &polygon, const LocationComponent &location, Vector2 axis, std::vector<Vector2> &vertices) {
    auto position = location.linear;
#ifdef USE_ROTATION
    auto angle = location.angular;
#endif
    auto size = polygon.count;
    auto localVertices = polygon.vertices;
    real maxProjection = REAL_MIN;
    real projection;
    for (size_t i = 0; i < size; ++i) {
#ifdef USE_ROTATION
        auto vertex = localVertices[i].getRotated(angle);
#else
        auto vertex = localVertices[i];
#endif
        projection = Vector2::dotProduct(vertex, axis);
        if(projection >= maxProjection - Epsilon) {
            if(!almostEqualsr(projection, maxProjection)) {
                maxProjection = projection;
                vertices.clear();
            }
            vertices.push_back(vertex + position);
        }
    }
}

void CollisionSystem::getMinMaxProjections(const std::vector<Vector2> &vertices, Vector2 axis, real &min, size_t &minIndex, real &max, size_t &maxIndex) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto projection = Vector2::dotProduct(vertices[i], axis);
        if(projection <= min) {
            min = projection;
            minIndex = i;
        }
        if(projection > max) {
            max = projection;
            maxIndex = i;
        }
    }
}

Vector2 CollisionSystem::getCollisionPointFromVertices(const std::vector<Vector2> &vertices1, const std::vector<Vector2> &vertices2, const Collision &collision) {
    if(vertices1.size() > 1) {
        if(vertices2.size() > 1) {
            Vector2 tangential = {collision.normal.y, -collision.normal.x};
            size_t min1Index, max1Index, min2Index, max2Index;
            real min1(REAL_MAX), max1(REAL_MIN), min2(REAL_MAX), max2(REAL_MIN);
            getMinMaxProjections(vertices1, tangential, min1, min1Index, max1, max1Index);
            getMinMaxProjections(vertices2, tangential, min2, min2Index, max2, max2Index);
            bool pickFirstMin = min1 > min2, pickFirstMax = max1 < max2;
            auto min = pickFirstMin ? vertices1[min1Index] : vertices2[min2Index];
            auto max = pickFirstMax ? vertices1[max1Index] : vertices2[max2Index];
            if(pickFirstMin) {
                if(pickFirstMax) {
                    return (min + max) * real(0.5) + collision.normal * (collision.penetration * real(0.5));
                } else {
                    return (min + max) * real(0.5);
                }
            } else {
                if(pickFirstMax) {
                    return (min + max) * real(0.5);
                } else {
                    return (min + max) * real(0.5) - collision.normal * (collision.penetration * real(0.5));
                }
            }
        } else {
            return vertices2[0] - collision.normal * (collision.penetration * real(0.5));
        }
    } else {
        if(vertices2.size() > 1) {
            return vertices1[0] + collision.normal * (collision.penetration * real(0.5));
        } else {
            return (vertices1[0] + vertices2[0]) * real(0.5);
        }
    }
}

Vector2 CollisionSystem::Convex2Convex(const Context &context, const Collision &collision) {
    auto &location1 = context.getComponent<LocationComponent>(collision.first);
    auto &polygon1 = context.getComponent<PolygonComponent>(collision.first);
    auto &location2 = context.getComponent<LocationComponent>(collision.second);
    auto &polygon2 = context.getComponent<PolygonComponent>(collision.second);

    std::vector<Vector2> maxVertices1, maxVertices2;
    getVerticesWithMaxProjection(polygon1, location1, collision.normal, maxVertices1);
    getVerticesWithMaxProjection(polygon2, location2, -collision.normal, maxVertices2);

    return getCollisionPointFromVertices(maxVertices1, maxVertices2, collision);
}
#endif
#ifndef USE_CIRCLES_ONLY
Vector2 CollisionSystem::AABB2AABB(const Context &context, const Collision &collision) {
    auto &shape1 = context.getComponent<ShapeComponent>(collision.first);
    auto &shape2 = context.getComponent<ShapeComponent>(collision.second);
    auto &aabb1 = shape1.boundingBox;
    auto &aabb2 = shape2.boundingBox;

    auto axis = collision.normal;
    if(axis.x != 0) {
        auto y1 = std::max(aabb1.min.y, aabb2.min.y);
        auto y2 = std::min(aabb1.max.y, aabb2.max.y);
        auto y = (y1 + y2) * real(0.5);
        if(axis.x < 0) {
            auto x = (aabb1.max.x + aabb2.min.x) * real(0.5);
            return {x, y};
        } else {
            auto x = (aabb1.min.x + aabb2.max.x) * real(0.5);
            return {x, y};
        }
    } else {
        auto x1 = std::max(aabb1.min.x, aabb2.min.x);
        auto x2 = std::max(aabb1.max.x, aabb2.max.x);
        auto x = (x1 + x2) * real(0.5);
        if(axis.y < 0) {
            auto y = (aabb1.max.y + aabb2.min.y) * real(0.5);
            return {x, y};
        } else {
            auto y = (aabb1.min.y + aabb2.max.y) * real(0.5);
            return {x, y};
        }
    }
}
#ifndef USE_AABB_ONLY
Vector2 CollisionSystem::Circle2AABB(const Context &context, const Collision &collision) {
    return AABB2Circle(context, {collision.second, collision.first, collision.penetration, -collision.normal});
}
Vector2 CollisionSystem::AABB2Circle(const Context &context, const Collision &collision) {
    auto &shape2 = context.getComponent<ShapeComponent>(collision.second);
    auto displacement = collision.normal * -(shape2.radius - collision.penetration * real(0.5));
    return shape2.centroid - displacement;
}
#endif
#ifndef USE_PRIMITIVES_ONLY
Vector2 CollisionSystem::AABB2Convex(const Context &context, const Collision &collision) {
    return Convex2AABB(context, {collision.second, collision.first, collision.penetration, -collision.normal});
}
Vector2 CollisionSystem::Convex2AABB(const Context &context, const Collision &collision) {
    auto &location1 = context.getComponent<LocationComponent>(collision.first);
    auto &polygon1 = context.getComponent<PolygonComponent>(collision.first);
    auto &shape2 = context.getComponent<ShapeComponent>(collision.second);
    auto aabb2 = shape2.boundingBox;

    Vector2 aabb2Vertices[4] = {aabb2.max, {aabb2.min.x, aabb2.max.y}, aabb2.min, {aabb2.max.x, aabb2.min.y}};
    auto center2 = shape2.getCenter();

    std::vector<Vector2> maxVertices1, maxVertices2;
    getVerticesWithMaxProjection(polygon1, location1, -collision.normal, maxVertices1);
    auto axis2 = collision.normal;
    real maxProjection = REAL_MIN;
    real projection;
    for (auto &vertex : aabb2Vertices) {
        projection = Vector2::dotProduct(vertex - center2, axis2);
        if(projection >= maxProjection - Epsilon) {
            if(!almostEqualsr(projection, maxProjection)) {
                maxProjection = projection;
                maxVertices2.clear();
            }
            maxVertices2.push_back(vertex);
        }
    }

    return getCollisionPointFromVertices(maxVertices1, maxVertices2, collision);
}
#endif
#endif
#ifndef USE_AABB_ONLY
Vector2 CollisionSystem::Circle2Circle(const Context &context, const Collision &collision) {
    auto &shape1 = context.getComponent<ShapeComponent>(collision.first);
    auto displacement = collision.normal * (shape1.radius - collision.penetration * real(0.5));
    return shape1.centroid - displacement;
}
#ifndef USE_PRIMITIVES_ONLY
Vector2 CollisionSystem::Circle2Convex(const Context &context, const Collision &collision) {
    return Convex2Circle(context, {collision.second, collision.first, collision.penetration, -collision.normal});
}
Vector2 CollisionSystem::Convex2Circle(const Context &context, const Collision &collision) {
    auto &shape2 = context.getComponent<ShapeComponent>(collision.second);
    auto displacement = collision.normal * -(shape2.radius - collision.penetration * real(0.5));
    return shape2.centroid - displacement;
}
#endif
#endif

#ifdef NEED_COLLISION_MATERIAL_INFO
#include <cmath>
CollisionSystem::CollisionMaterialInfo CollisionSystem::getMaterialInfo(const Context &context, EntityId id1, EntityId id2) {
    CollisionMaterialInfo info{};
    if(context.hasComponent<MaterialComponent>(id1)) {
        info.hasInfo = true;
        auto &material1 = context.getComponent<MaterialComponent>(id1);
#ifdef USE_BOUNCINESS
        info.bounciness = material1.bounciness;
#endif
#ifdef USE_FRICTION
        info.friction = material1.friction;
#endif
        if(context.hasComponent<MaterialComponent>(id2)) {
            auto &material2 = context.getComponent<MaterialComponent>(id2);
#ifdef USE_BOUNCINESS
            auto e2 = material2.bounciness;
            if(e2 < info.bounciness) info.bounciness = e2;
#endif
#ifdef USE_FRICTION
            auto f2 = material2.friction;
            info.friction = sqrtr(info.friction * info.friction + f2 * f2);
#endif
        }
    } else {
        if(context.hasComponent<MaterialComponent>(id2)) {
            info.hasInfo = true;
            auto &material2 = context.getComponent<MaterialComponent>(id2);
#ifdef USE_BOUNCINESS
            info.bounciness = material2.bounciness;
#endif
#ifdef USE_FRICTION
            info.friction = material2.friction;
#endif
        }
    }
    return info;
}
#endif

void CollisionSystem::updateVelocities(const Context &context, EntityId id1, EntityId id2,
                                       Vector2 relativeVelocityVector, Vector2 direction,
                                       const MassInfoComponent &massInfo1, const MassInfoComponent &massInfo2,
                                       Vector2 r1, Vector2 r2,
                                       VelocityComponent &velocity1, VelocityComponent &velocity2) {
    auto relativeVelocity = Vector2::dotProduct(relativeVelocityVector, direction);
    auto needUpdateNormal = relativeVelocity > 0;
#if (!defined(USE_ROTATION) || !defined(USE_INERTIA)) && !defined(USE_FRICTION)
    if(!needUpdateNormal) return;
#endif
    auto denominator = massInfo1.inverseMass + massInfo2.inverseMass;
#if defined(USE_INERTIA) && !defined(USE_AABB_ONLY)
    auto needUseInertia1 = context.getComponent<ShapeComponent>(id1).shapeType != ShapeType::AABB;
    auto needUseInertia2 = context.getComponent<ShapeComponent>(id2).shapeType != ShapeType::AABB;
    if(needUseInertia1) denominator += Vector2::crossProduct(r1, direction * (massInfo1.inverseInertia * Vector2::crossProduct(r1, direction)));
    if(needUseInertia2) denominator += Vector2::crossProduct(r2, direction * (massInfo2.inverseInertia * Vector2::crossProduct(r2, direction)));
#endif
#ifdef NEED_COLLISION_MATERIAL_INFO
    auto matInfo = getMaterialInfo(context, id1, id2);
#endif
#ifdef NEED_COLLISION_MATERIAL_INFO
#ifdef USE_BOUNCINESS
        auto numerator = matInfo.hasInfo ? direction * ((1 + matInfo.bounciness) * relativeVelocity) :
                         direction * relativeVelocity;
#else
        auto numerator = direction * relativeVelocity;
#endif
#else
        auto numerator = direction * relativeVelocity;
#endif
        auto impulse = numerator / denominator;
    if(needUpdateNormal) {
        velocity1.linear += impulse * massInfo1.inverseMass;
        velocity2.linear += impulse * -massInfo2.inverseMass;
    }
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        if(needUseInertia1) velocity1.angular += massInfo1.inverseInertia * Vector2::crossProduct(r1, impulse);
        if(needUseInertia2) velocity2.angular += -massInfo2.inverseInertia * Vector2::crossProduct(r2, impulse);
#endif
#ifdef USE_FRICTION
    if(matInfo.hasInfo) {
        auto relativeTangentialVelocityVector = relativeVelocityVector - (direction * relativeVelocity);
        auto tangential = relativeTangentialVelocityVector.getNormalized();
        if(std::isnan(tangential.x)) return;
        auto tangentialNumerator = relativeTangentialVelocityVector * (1 + matInfo.bounciness);
#if defined(USE_INERTIA) && !defined(USE_AABB_ONLY)
        auto tangentialDenominator = massInfo1.inverseMass + massInfo2.inverseMass;
        if(needUseInertia1) tangentialDenominator += Vector2::crossProduct(r1, tangential * (massInfo1.inverseInertia * Vector2::crossProduct(r1, tangential)));
        if(needUseInertia2) tangentialDenominator += Vector2::crossProduct(r2, tangential * (massInfo2.inverseInertia * Vector2::crossProduct(r2, tangential)));
#else
        auto tangentialDenominator = denominator;
#endif
        auto tangentialImpulse = tangentialNumerator / tangentialDenominator;
        velocity1.linear += tangentialImpulse * (massInfo1.inverseMass * matInfo.friction);
        velocity2.linear += tangentialImpulse * (-massInfo2.inverseMass * matInfo.friction);
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        if(needUseInertia1) velocity1.angular += massInfo1.inverseInertia * matInfo.friction * Vector2::crossProduct(r1, tangentialImpulse);
        if(needUseInertia2) velocity2.angular += -massInfo2.inverseInertia * matInfo.friction * Vector2::crossProduct(r2, tangentialImpulse);
#endif
    }
#endif
}

void CollisionSystem::updateVelocity(const Context &context, EntityId id, Vector2 relativeVelocityVector, Vector2 direction,
                                     const MassInfoComponent &massInfo, Vector2 r, VelocityComponent &velocity) {

    auto relativeVelocity = Vector2::dotProduct(relativeVelocityVector, direction);
    auto needUpdateNormal = relativeVelocity > 0;
#if (!defined(USE_ROTATION) || !defined(USE_INERTIA)) && !defined(USE_FRICTION)
    if(!needUpdateNormal) return;
#endif
    auto denominator = massInfo.inverseMass;
#if defined(USE_INERTIA) && !defined(USE_AABB_ONLY)
    auto needUseInertia = context.getComponent<ShapeComponent>(id).shapeType != ShapeType::AABB;
    if(needUseInertia) denominator += Vector2::crossProduct(r, direction * (massInfo.inverseInertia * Vector2::crossProduct(r, direction)));
#endif
#ifdef NEED_COLLISION_MATERIAL_INFO
    auto hasMaterial = context.hasComponent<MaterialComponent>(id);
    auto &material = context.getComponent<MaterialComponent>(id);
#endif
#ifdef NEED_COLLISION_MATERIAL_INFO
#ifdef USE_BOUNCINESS
        auto numerator = hasMaterial ? direction * ((1 + material.bounciness) * relativeVelocity) :
                         direction * relativeVelocity;
#else
        auto numerator = direction * relativeVelocity;
#endif
#else
        auto numerator = direction * relativeVelocity;
#endif
        auto impulse = numerator / denominator;
    if(needUpdateNormal) velocity.linear += impulse * massInfo.inverseMass;
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        if(needUseInertia) velocity.angular += massInfo.inverseInertia * Vector2::crossProduct(r, impulse);
#endif
#ifdef USE_FRICTION
    if(hasMaterial) {
        auto relativeTangentialVelocityVector = relativeVelocityVector - (direction * relativeVelocity);
        auto tangential = relativeTangentialVelocityVector.getNormalized();
        if(std::isnan(tangential.x)) return;
        auto tangentialNumerator = relativeTangentialVelocityVector * (1 + material.bounciness);
#if defined(USE_INERTIA) && !defined(USE_AABB_ONLY)
        auto tangentialDenominator = massInfo.inverseMass;
        if(needUseInertia) tangentialDenominator += Vector2::crossProduct(r, tangential * (massInfo.inverseInertia * Vector2::crossProduct(r, tangential)));
#else
        auto tangentialDenominator = denominator;
#endif
        auto tangentialImpulse = tangentialNumerator / tangentialDenominator;
        velocity.linear += tangentialImpulse * (massInfo.inverseMass * material.friction);
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        if(needUseInertia) velocity.angular += massInfo.inverseInertia * material.friction * Vector2::crossProduct(r, tangentialImpulse);
#endif
    }
#endif
}

Vector2 CollisionSystem::getCollisionPoint(const Context &context, const Collision &collision) {
    auto &shape1 = context.getComponent<ShapeComponent>(collision.first);
    auto &shape2 = context.getComponent<ShapeComponent>(collision.second);

    handler_function handlerFunc = handlers[static_cast<size_t>(shape1.shapeType)][static_cast<size_t>(shape2.shapeType)];
    return (*handlerFunc)(context, collision);
}

void CollisionSystem::scaleRelativeVelocity(Vector2 &velocity, const Vector2 &normal, real penetrationVelocity) {
    auto normalVelocity = Vector2::dotProduct(velocity, normal);
    if(normalVelocity >= penetrationVelocity - Epsilon || normalVelocity <= Epsilon) return;
    velocity += normal * (penetrationVelocity - normalVelocity);
}
#endif

void CollisionSystem::update(Context &context, EntityId id, real deltaTime) { }

void CollisionSystem::update(Context &context, real deltaTime) {
    for(auto &collision : context.collisions) {
        auto id1 = collision.first;
        auto id2 = collision.second;
        auto direction = collision.normal;
        auto magnitude = collision.penetration;
        auto &location1 = context.getComponent<LocationComponent>(id1);
        auto &location2 = context.getComponent<LocationComponent>(id2);
#ifdef USE_COLLISION_HOOKE
        auto force = k * magnitude;
#elif defined(USE_COLLISION_IMPULSE)
        auto collisionPoint = getCollisionPoint(context, collision);
#ifdef USE_INERTIA
        auto r1 = collisionPoint - location1.linear;
        auto r2 = collisionPoint - location2.linear;
#else
        const Vector2 r1{}, r2{};
#endif
#endif

        if(context.hasComponent<MassInfoComponent>(id1)) {
            auto &massInfo1 = context.getComponent<MassInfoComponent>(id1);
#ifdef USE_COLLISION_HOOKE
            auto dV1 = direction * force * massInfo1.inverseMass * deltaTime;
#endif
            if(context.hasComponent<MassInfoComponent>(id2)) {
                auto &massInfo2 = context.getComponent<MassInfoComponent>(id2);
#ifdef USE_COLLISION_HOOKE
                auto dV2 = -direction * force * massInfo2.inverseMass * deltaTime;
#endif
#ifdef NEED_VELOCITY_CHECK
                if(context.hasComponent<VelocityComponent>(id1)) {
                    auto &velocity1 = context.getComponent<VelocityComponent>(id1);
                    if(context.hasComponent<VelocityComponent>(id2)) {
                        auto &velocity2 = context.getComponent<VelocityComponent>(id2);
                        //m1, m2, v1, v2
#ifdef USE_COLLISION_HOOKE
                        velocity1.linear += dV1;
                        velocity2.linear += dV2;
#else
                        auto relativeVelocityVector = velocity2.linear - velocity1.linear;
#ifdef USE_ROTATION
                        relativeVelocityVector -= -Vector2{r1.y, -r1.x} * velocity1.angular;
                        relativeVelocityVector += -Vector2{r2.y, -r2.x} * velocity2.angular;
#endif
                        scaleRelativeVelocity(relativeVelocityVector, direction, magnitude / deltaTime);
                        updateVelocities(context, id1, id2, relativeVelocityVector, direction,
                                         massInfo1, massInfo2, r1, r2, velocity1, velocity2);
#endif
                    } else {
                        //m1, m2, v1
#ifdef USE_COLLISION_HOOKE
                        velocity1.linear += dV1;
                        context.addComponent<VelocityComponent>(id2, dV2);
#else
                        VelocityComponent velocity2 {Vector2{}};
                        auto relativeVelocityVector = -velocity1.linear;
#ifdef USE_ROTATION
                        relativeVelocityVector -= -Vector2{r1.y, -r1.x} * velocity1.angular;
#endif
                        scaleRelativeVelocity(relativeVelocityVector, direction, magnitude / deltaTime);
                        updateVelocities(context, id1, id2, relativeVelocityVector, direction,
                                         massInfo1, massInfo2, r1, r2, velocity1, velocity2);
                        context.addComponent<VelocityComponent>(id2, velocity2);
#endif
                    }
                } else {
                    if(context.hasComponent<VelocityComponent>(id2)) {
                        auto &velocity2 = context.getComponent<VelocityComponent>(id2);
                        //m1, m2, v2
#ifdef USE_COLLISION_HOOKE
                        context.addComponent<VelocityComponent>(id1, dV1);
                        velocity2.linear += dV2;
#else
                        VelocityComponent velocity1 {Vector2{}};
                        auto relativeVelocityVector = velocity2.linear;
#ifdef USE_ROTATION
                        relativeVelocityVector += -Vector2{r2.y, -r2.x} * velocity2.angular;
#endif
                        scaleRelativeVelocity(relativeVelocityVector, direction, magnitude / deltaTime);
                        updateVelocities(context, id1, id2, relativeVelocityVector, direction,
                                         massInfo1, massInfo2, r1, r2, velocity1, velocity2);
                        context.addComponent<VelocityComponent>(id1, velocity1);
#endif
                    } else
#endif
                    {
                        //m1, m2
#ifdef USE_COLLISION_DISPLACEMENT
#ifdef USE_EQUAL_MASSES
                        auto displacement = direction * (magnitude * real(0.5));
                        location1.linear += displacement;
                        location2.linear -= displacement;
#else
                        auto totalMass = massInfo1.mass + massInfo2.mass;
                        location1.linear += direction * (magnitude * massInfo2.mass / totalMass);
                        location2.linear -= direction * (magnitude * massInfo1.mass / totalMass);
#endif
#elif defined(USE_COLLISION_HOOKE)
                        context.addComponent<VelocityComponent>(id1, dV1);
                        context.addComponent<VelocityComponent>(id2, dV2);
#elif defined(USE_COLLISION_IMPULSE)
                        VelocityComponent velocity1 {Vector2{}}, velocity2 {Vector2{}};
                        auto relativeVelocityVector = direction * (-magnitude / deltaTime);
                        updateVelocities(context, id1, id2, relativeVelocityVector, direction,
                                         massInfo1, massInfo2, r1, r2, velocity1, velocity2);
                        context.addComponent<VelocityComponent>(id1, velocity1);
                        context.addComponent<VelocityComponent>(id2, velocity2);
#else
#warning Unknown collision solver!
#endif
                    }
#ifdef NEED_VELOCITY_CHECK
                }
#endif
            } else {
#ifdef NEED_VELOCITY_CHECK
                if(context.hasComponent<VelocityComponent>(id1)) {
                    auto &velocity1 = context.getComponent<VelocityComponent>(id1);
                    //m1, v1
#ifdef USE_COLLISION_HOOKE
                    velocity1.linear += dV1;
#else
                    auto relativeVelocityVector = -velocity1.linear;
#ifdef USE_ROTATION
                    relativeVelocityVector -= -Vector2{r1.y, -r1.x} * velocity1.angular;
#endif
                    scaleRelativeVelocity(relativeVelocityVector, direction, magnitude / deltaTime);
                    updateVelocity(context, id1, relativeVelocityVector, direction, massInfo1, r1, velocity1);
#endif
                } else
#endif
                {
                    //m1
#ifdef USE_COLLISION_DISPLACEMENT
                    location1.linear += direction * magnitude;
#elif defined(USE_COLLISION_HOOKE)
                    context.addComponent<VelocityComponent>(id1, dV1);
#elif defined(USE_COLLISION_IMPULSE)
                    VelocityComponent velocity1 {Vector2{}};
                    auto relativeVelocityVector = direction * (-magnitude / deltaTime);
                    updateVelocity(context, id1, relativeVelocityVector, direction, massInfo1, r1, velocity1);
                    context.addComponent<VelocityComponent>(id1, velocity1);
#else
#warning Unknown collision solver!
#endif
                }
            }
        } else {
            if(context.hasComponent<MassInfoComponent>(id2)) {
                auto &massInfo2 = context.getComponent<MassInfoComponent>(id2);
#ifdef USE_COLLISION_HOOKE
                auto dV2 = -direction * force * massInfo2.inverseMass * deltaTime;
#endif
#ifdef NEED_VELOCITY_CHECK
                if(context.hasComponent<VelocityComponent>(id2)) {
                    auto &velocity2 = context.getComponent<VelocityComponent>(id2);
                    //m2, v2
#ifdef USE_COLLISION_HOOKE
                    velocity2.linear += dV2;
#else
                    auto relativeVelocityVector = -velocity2.linear;
#ifdef USE_ROTATION
                    relativeVelocityVector -= -Vector2{r2.y, -r2.x} * velocity2.angular;
#endif
                    scaleRelativeVelocity(relativeVelocityVector, -direction, magnitude / deltaTime);
                    updateVelocity(context, id2, relativeVelocityVector, -direction, massInfo2, r2, velocity2);
#endif
                } else
#endif
                {
                    //m2
#ifdef USE_COLLISION_DISPLACEMENT
                    location2.linear -= direction * magnitude;
#elif defined(USE_COLLISION_HOOKE)
                    context.addComponent<VelocityComponent>(id2, dV2);
#elif defined(USE_COLLISION_IMPULSE)
                    VelocityComponent velocity2 {Vector2{}};
                    auto relativeVelocityVector = direction * (-magnitude / deltaTime);
                    updateVelocity(context, id2, relativeVelocityVector, -direction, massInfo2, r2, velocity2);
                    context.addComponent<VelocityComponent>(id2, velocity2);
#else
#warning Unknown collision solver!
#endif
                }
            } else {
                //none
                auto displacement = direction * (magnitude * real(0.5));
                location1.linear += displacement;
                location2.linear -= displacement;
            }
        }
    }
    context.collisions.clear();
}

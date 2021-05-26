#ifndef ADJUSTABLEPHYSICS2D_COLLISIONSYSTEM_H
#define ADJUSTABLEPHYSICS2D_COLLISIONSYSTEM_H

#include "System.h"

class CollisionSystem : public System {
private:
#ifdef USE_COLLISION_IMPULSE
#if defined(USE_FRICTION) || defined(USE_BOUNCINESS)
#define NEED_COLLISION_MATERIAL_INFO
    struct CollisionMaterialInfo {
        bool hasInfo;
#ifdef USE_FRICTION
        real friction;
#endif
#ifdef USE_BOUNCINESS
        real bounciness;
#endif
    };
    static CollisionMaterialInfo getMaterialInfo(const Context &context, EntityId id1, EntityId id2);
#endif
    static void updateVelocities(const Context &context, EntityId id1, EntityId id2,
                                 Vector2 relativeVelocityVector, Vector2 direction,
                                 MassInfoComponent &massInfo1, MassInfoComponent &massInfo2,
                                 Vector2 r1, Vector2 r2,
                                 VelocityComponent &velocity1, VelocityComponent &velocity2);
    static void updateVelocity(const Context &context, EntityId id,
                               Vector2 relativeVelocityVector, Vector2 direction,
                               MassInfoComponent &massInfo,
                               Vector2 r,
                               VelocityComponent &velocity);
    Vector2 getCollisionPoint(const Context &context, const Collision &collision);
    using handler_function = Vector2 (*)(const Context&, const Collision&);
    handler_function handlers[3][3];
#ifndef USE_PRIMITIVES_ONLY
    static void getVerticesWithMaxProjection(const PolygonComponent &polygon, const LocationComponent &location, Vector2 axis, std::vector<Vector2> &edges);
    static void getMinMaxProjections(const std::vector<Vector2> &vertices, Vector2 axis, real &min, size_t &minIndex, real &max, size_t &maxIndex);
    static Vector2 getCollisionPointFromVertices(const std::vector<Vector2> &vertices1, const std::vector<Vector2> &vertices2, const Collision &collision);
    static Vector2 Convex2Convex(const Context &context, const Collision &collision);
#endif
#ifndef USE_CIRCLES_ONLY
    static Vector2 AABB2AABB(const Context &context, const Collision &collision);
#ifndef USE_AABB_ONLY
    static Vector2 AABB2Circle(const Context &context, const Collision &collision);
    static Vector2 Circle2AABB(const Context &context, const Collision &collision);
#endif
#ifndef USE_PRIMITIVES_ONLY
    static Vector2 Convex2AABB(const Context &context, const Collision &collision);
    static Vector2 AABB2Convex(const Context &context, const Collision &collision);
#endif
#endif
#ifndef USE_AABB_ONLY
    static Vector2 Circle2Circle(const Context &context, const Collision &collision);
#ifndef USE_PRIMITIVES_ONLY
    static Vector2 Convex2Circle(const Context &context, const Collision &collision);
    static Vector2 Circle2Convex(const Context &context, const Collision &collision);
#endif
#endif
#endif
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
#ifdef USE_COLLISION_HOOKE
    real k;
#endif
    CollisionSystem();
    void update(Context &context, real deltaTime) override;
};

#endif //ADJUSTABLEPHYSICS2D_COLLISIONSYSTEM_H

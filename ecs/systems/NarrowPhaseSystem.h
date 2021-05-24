#ifndef ADJUSTABLEPHYSICS2D_NARROWPHASESYSTEM_H
#define ADJUSTABLEPHYSICS2D_NARROWPHASESYSTEM_H

#include "System.h"

class NarrowPhaseSystem : public System {
private:
    using handler_function = void (*)(Context&, EntityId, EntityId, real);
    handler_function handlers[3][3];
    void handle(Context &context, EntityId id1, EntityId id2, real deltaTime);
#ifndef USE_PRIMITIVES_ONLY
    static bool checkPenetration(real min1, real max1, real min2, real max2, const Vector2& axis, Vector2& normal, real& penetration);
#ifdef USE_ROTATION
    static void getGlobalVectors(const Polygon& polygon, const Vector2& position, real angle, Vector2 edges[], Vector2 normals[]);
#else
    static void getGlobalVectors(const Polygon& polygon, const Vector2& location, Vector2 edges[], Vector2 normals[]);
#endif
    static void getMinMaxOnAxis(const Vector2 edges[], size_t count, const Vector2& axis, real& min, real& max);
    static void Convex2Convex(Context &context, EntityId id1, EntityId id2, real deltaTime);
#endif
#ifndef USE_CIRCLES_ONLY
    static void AABB2AABB(Context &context, EntityId id1, EntityId id2, real deltaTime);
#ifndef USE_AABB_ONLY
    static void AABB2Circle(Context &context, EntityId id1, EntityId id2, real deltaTime);
    static void Circle2AABB(Context &context, EntityId id1, EntityId id2, real deltaTime);
#endif
#ifndef USE_PRIMITIVES_ONLY
    static void Convex2AABB(Context &context, EntityId id1, EntityId id2, real deltaTime);
    static void AABB2Convex(Context &context, EntityId id1, EntityId id2, real deltaTime);
#endif
#endif
#ifndef USE_AABB_ONLY
    static void Circle2Circle(Context &context, EntityId id1, EntityId id2, real deltaTime);
#ifndef USE_PRIMITIVES_ONLY
    static void getMinMaxOnAxis(const Vector2& center, real radius, const Vector2& axis, real& min, real& max);
    static void Convex2Circle(Context &context, EntityId id1, EntityId id2, real deltaTime);
    static void Circle2Convex(Context &context, EntityId id1, EntityId id2, real deltaTime);
#endif
#endif
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    NarrowPhaseSystem();
    void update(Context &context, real deltaTime) override;
};

#endif //ADJUSTABLEPHYSICS2D_NARROWPHASESYSTEM_H

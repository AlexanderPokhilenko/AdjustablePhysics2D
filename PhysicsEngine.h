#ifndef ADJUSTABLEPHYSICS2D_PHYSICSENGINE_H
#define ADJUSTABLEPHYSICS2D_PHYSICSENGINE_H

#include "ecs/Context.h"
#include "ecs/systems/System.h"
#include "ecs/systems/SystemType.h"
#include "Entity.h"
#include <functional>

class PhysicsEngine {
private:
    Context context;
    System* systems[SystemsCount];
public:
#ifdef USE_INERTIA
    static constexpr const real CylinderInertiaCoefficient = real(1) / real(2);
    static constexpr const real HoopInertiaCoefficient = real(1);
    static constexpr const real SolidSphereInertiaCoefficient = real(2) / real(5);
    static constexpr const real ThinSphereInertiaCoefficient = real(2) / real(3);
    static constexpr const real DefaultInertiaCoefficient = SolidSphereInertiaCoefficient;
#endif
    explicit PhysicsEngine(size_t entitiesCapacity = 64, size_t freeCapacity = 16);
    void forEachEntity(const std::function<void(Entity&)>&);
    bool hasEntity(EntityId id);
    Entity getEntity(EntityId id);
    void deleteEntity(EntityId id);
    static void deleteEntity(Entity entity);
#ifdef USE_JOINT
    void forEachJoint(const std::function<void(Joint&, Entity&, Entity&)>&);
    void createJoint(EntityId id1, EntityId id2, real kSpring, real kDamper, real length, Vector2 point1 = Vector2(), Vector2 point2 = Vector2());
    void createJoint(EntityId id1, EntityId id2, real kSpring, real kDamper, Vector2 point1 = Vector2(), Vector2 point2 = Vector2());
    void createJoint(EntityId id1, EntityId id2, real kSpring, real kDamper = 0);
    DoubleKeyContainer<EntityId, Joint>& getJoints();
#endif
#ifndef USE_PRIMITIVES_ONLY
    Entity createComplex(Vector2* edges, size_t count, Transform location);
#endif
#ifndef USE_CIRCLES_ONLY
    Entity createAABB(Vector2 min, Vector2 max, Transform location);
#endif
#ifndef USE_AABB_ONLY
    Entity createCircle(real radius, Transform location);
#endif
    void simulate(real deltaTime);
    ~PhysicsEngine();
};

#endif //ADJUSTABLEPHYSICS2D_PHYSICSENGINE_H

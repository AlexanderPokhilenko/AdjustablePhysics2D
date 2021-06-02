#ifndef ADJUSTABLEPHYSICS2D_ENTITY_H
#define ADJUSTABLEPHYSICS2D_ENTITY_H

#include "base/primitives.h"
#include "ecs/components/ComponentType.h"
#include "ecs/Context.h"

struct Entity {
friend class PhysicsEngine;
private:
    Context &context;
public:
    const EntityId id;
    Entity(Context &entityContext, EntityId entityId);
    explicit operator EntityId() const;
    static Entity create(Context &context);
    bool exists() const;
    bool hasComponent(ComponentType type) const;
    template<typename T>
    const T& getComponent() const {
        return context.getComponent<T>(id);
    }
    void removeComponent(ComponentType type);
#ifndef USE_PRIMITIVES_ONLY
    void makeConvex(const Vector2 vertices[], size_t count);
    void makeComplexFromAABB(Vector2 min, Vector2 max);
    void makeComplexFromAABB();
#endif
#ifndef USE_CIRCLES_ONLY
    void makeAABB(Vector2 min, Vector2 max);
#endif
#ifndef USE_AABB_ONLY
    void makeCircle(real radius);
#endif
#ifndef USE_EQUAL_MASSES
    void setMass(real mass);
#endif
#ifdef USE_INERTIA
    void setInertia(real inertia);
#ifndef USE_EQUAL_MASSES
    void setMassInfo(real mass, real inertia);
#endif
#endif
#ifdef USE_MATERIAL
    void setMaterial(Material material);
#endif
    void setLocation(Transform location);
    void setVelocity(Transform velocity);
    void addImpulse(Vector2 linear
#if defined(USE_ROTATION) && defined(USE_INERTIA)
            , real angular = 0
#endif
    );
#ifdef USE_ACCELERATION
    void setAcceleration(Transform acceleration);
    void addForce(Vector2 force
#if defined(USE_ROTATION) && defined(USE_INERTIA)
    , real torque = 0
#endif
    );
#endif
#ifdef USE_COLLISION_FILTER
    void setCategory(const CollisionFilterBitset &category);
    void addCategory(const CollisionFilterBitset &category);
    void removeCategory(const CollisionFilterBitset &category);
    void setMask(const CollisionFilterBitset &mask);
    void addMask(const CollisionFilterBitset &mask);
    void removeMask(const CollisionFilterBitset &mask);
#endif
};

#endif //ADJUSTABLEPHYSICS2D_ENTITY_H

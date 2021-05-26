#ifndef ADJUSTABLEPHYSICS2D_ENTITY_H
#define ADJUSTABLEPHYSICS2D_ENTITY_H

#include "base/primitives.h"
#include "ecs/components/ComponentType.h"
#include "ecs/Context.h"

#if defined(USE_DENSITY) || defined(USE_FRICTION) || defined(USE_FRICTION)
#define USE_MATERIAL
#endif

struct Entity {
friend class PhysicsEngine;
private:
    Context &context;
    const EntityId id;
public:
    Entity(Context &entityContext, EntityId entityId);
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
    void setAcceleration(Transform acceleration);
};

#endif //ADJUSTABLEPHYSICS2D_ENTITY_H

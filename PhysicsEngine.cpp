#include "PhysicsEngine.h"
#include "ecs/systems/MoveSystem.h"
#ifdef USE_GLOBAL_GRAVITATION
#include "ecs/systems/GlobalGravitationSystem.h"
#endif
#ifdef USE_GRAVITATIONAL_FORCE
#include "ecs/systems/GravitationalForceSystem.h"
#endif
#include "ecs/systems/ShapeTranslationSystem.h"
#ifdef USE_BROAD_PHASE
#include "ecs/systems/BroadPhaseSystem.h"
#endif
#include "ecs/systems/NarrowPhaseSystem.h"
#include "ecs/systems/CollisionSystem.h"

PhysicsEngine::PhysicsEngine(size_t entitiesCapacity, size_t freeCapacity) : context(entitiesCapacity, freeCapacity),
systems() {
    systems[static_cast<std::size_t>(SystemType::Move)] = new MoveSystem();
#ifdef USE_GLOBAL_GRAVITATION
    systems[static_cast<std::size_t>(SystemType::GlobalGravitation)] = new GlobalGravitationSystem();
#endif
#ifdef USE_GRAVITATIONAL_FORCE
    systems[static_cast<std::size_t>(SystemType::GravitationalForce)] = new GravitationalForceSystem();
#endif
    systems[static_cast<std::size_t>(SystemType::ShapeTranslation)] = new ShapeTranslationSystem();
#ifdef USE_BROAD_PHASE
    systems[static_cast<std::size_t>(SystemType::BroadPhase)] = new BroadPhaseSystem();
#endif
    systems[static_cast<std::size_t>(SystemType::NarrowPhase)] = new NarrowPhaseSystem();
    systems[static_cast<std::size_t>(SystemType::Collision)] = new CollisionSystem();
}

void PhysicsEngine::simulate(real deltaTime) {
    for (auto & system : systems) {
        system->update(context, deltaTime);
    }
}

bool PhysicsEngine::hasEntity(EntityId id) {
    return context.hasEntity(id);
}

Entity PhysicsEngine::getEntity(EntityId id) {
    return {context, id};
}

void PhysicsEngine::deleteEntity(EntityId id) {
    return context.deleteEntity(id);
}

void PhysicsEngine::deleteEntity(Entity entity) {
    return entity.context.deleteEntity(entity.id);
}

#ifndef USE_PRIMITIVES_ONLY
Entity PhysicsEngine::createComplex(Vector2* edges, size_t count, Transform location) {
    auto entity = Entity::create(context);
    entity.makeConvex(edges, count);
    entity.setLocation(location);
    return entity;
}
#endif
#ifndef USE_CIRCLES_ONLY
Entity PhysicsEngine::createAABB(Vector2 min, Vector2 max, Transform location) {
    auto entity = Entity::create(context);
    entity.makeAABB(min, max);
    entity.setLocation(location);
    return entity;
}
#endif
#ifndef USE_AABB_ONLY
Entity PhysicsEngine::createCircle(real radius, Transform location) {
    auto entity = Entity::create(context);
    entity.makeCircle(radius);
    entity.setLocation(location);
    return entity;
}
#endif

PhysicsEngine::~PhysicsEngine() {
    for (auto & system : systems) {
        delete system;
    }
}

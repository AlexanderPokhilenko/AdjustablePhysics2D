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
#ifdef USE_JOINT
#include "ecs/systems/JointSystem.h"
#endif

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
#ifdef USE_JOINT
    systems[static_cast<std::size_t>(SystemType::Joint)] = new JointSystem();
#endif
}

void PhysicsEngine::forEachEntity(const std::function<void(Entity &)>& func) {
    auto size = context.getEntitiesSize();
    for (EntityId i = 0; i < size; i++) {
        if(context.hasEntity(i))
        {
            Entity entity{context, i};
            func(entity);
        }
    }
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
#ifdef USE_JOINT
    context.joints.tryRemoveAll(id);
#endif
    return context.deleteEntity(id);
}

void PhysicsEngine::deleteEntity(Entity entity) {
#ifdef USE_JOINT
    entity.context.joints.tryRemoveAll(entity.id);
#endif
    return entity.context.deleteEntity(entity.id);
}

#ifdef USE_JOINT
void PhysicsEngine::forEachJoint(const std::function<void(Joint&, Entity&, Entity&)>& func) {
    context.joints.forEach([this, &func](Joint &joint) {
        Entity e1{context, joint.id1}, e2{context, joint.id2};
        func(joint, e1, e2);
    });
}

void PhysicsEngine::createJoint(EntityId id1, EntityId id2, real kSpring, real kDamper, real length, Vector2 point1, Vector2 point2) {
    context.joints.add(id1, id2, id1, id2, kSpring, kDamper, length, point1, point2);
}

void PhysicsEngine::createJoint(EntityId id1, EntityId id2, real kSpring, real kDamper, Vector2 point1, Vector2 point2) {
    auto &location1 = context.getComponent<LocationComponent>(id1);
    auto &location2 = context.getComponent<LocationComponent>(id2);
    auto position1 = location1.linear;
    auto position2 = location2.linear;
#ifdef USE_ROTATION
    position1 += point1.getRotated(location1.angular);
    position2 += point2.getRotated(location2.angular);
#else
    position1 += point1;
    position2 += point2;
#endif
    createJoint(id1, id2, kSpring, kDamper, (position2 - position1).getMagnitude(), point1, point2);
}

void PhysicsEngine::createJoint(EntityId id1, EntityId id2, real kSpring, real kDamper) {
    createJoint(id1, id2, kSpring, kDamper, Vector2{0, 0}, Vector2{0, 0});
}

DoubleKeyContainer<EntityId, Joint>& PhysicsEngine::getJoints() {
    return context.joints;
}
#endif

#ifndef USE_PRIMITIVES_ONLY
Entity PhysicsEngine::createComplex(Vector2* vertices, size_t count, Transform location) {
    auto entity = Entity::create(context);
    entity.makeConvex(vertices, count);
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

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
}

void PhysicsEngine::simulate(real deltaTime) {
    for (auto & system : systems) {
        system->update(context, deltaTime);
    }
}

PhysicsEngine::~PhysicsEngine() {
    for (auto & system : systems) {
        delete system;
    }
}

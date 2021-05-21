#include "PhysicsEngine.h"
#include "ecs/systems/MoveSystem.h"
#ifdef USE_GLOBAL_GRAVITATION
#include "ecs/systems/GlobalGravitationSystem.h"
#endif

PhysicsEngine::PhysicsEngine(size_t entitiesCapacity, size_t freeCapacity) : context(entitiesCapacity, freeCapacity),
systems() {
    systems[static_cast<std::size_t>(SystemType::Move)] = new MoveSystem();
#ifdef USE_GLOBAL_GRAVITATION
    systems[static_cast<std::size_t>(SystemType::GlobalGravitation)] = new GlobalGravitationSystem();
#endif
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

#include "PhysicsEngine.h"
#include "ecs/systems/MoveSystem.h"

PhysicsEngine::PhysicsEngine(size_t entitiesCapacity, size_t freeCapacity) : context(entitiesCapacity, freeCapacity),
systems() {
    systems[static_cast<std::size_t>(SystemType::MoveSystem)] = new MoveSystem();
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

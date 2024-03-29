#include "GlobalGravitationSystem.h"

ComponentsBitset GlobalGravitationSystem::createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Velocity));
    bitset.set(static_cast<size_t>(ComponentType::MassInfo));
    return bitset;
}

GlobalGravitationSystem::GlobalGravitationSystem() : System(createCurrentSystemBitset()), g({0, 9.81}) { }

void GlobalGravitationSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &velocity = context.getComponent<VelocityComponent>(id);
    velocity.linear += g * deltaTime;
}

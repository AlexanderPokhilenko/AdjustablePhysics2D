#include "GravitationalForceSystem.h"

ComponentsBitset GravitationalForceSystem::createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Location));
    bitset.set(static_cast<size_t>(ComponentType::Velocity));
    bitset.set(static_cast<size_t>(ComponentType::MassInfo));
    return bitset;
}

GravitationalForceSystem::GravitationalForceSystem() : System(createCurrentSystemBitset()), G(6.6743e-11) { }

void GravitationalForceSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &position1 = context.getComponent<LocationComponent>(id).linear;
    auto &velocity1 = context.getComponent<VelocityComponent>(id);
    auto &massInfo1 = context.getComponent<MassInfoComponent>(id);
#ifndef USE_QUADTREE
    auto entitiesSize = context.getEntitiesSize();
    for (size_t i = id + 1; i < entitiesSize; ++i) {
        if(!context.checkEntity(i, componentsBitset)) continue;
        auto &position2 = context.getComponent<LocationComponent>(i).linear;
        auto &velocity2 = context.getComponent<VelocityComponent>(i);
        auto &massInfo2 = context.getComponent<MassInfoComponent>(i);
        auto displacement = position2 - position1;
        auto direction = displacement.getNormalized();
        auto force = G * massInfo1.mass * massInfo2.mass / displacement.getSqrMagnitude();
        velocity1.linear += direction * force * massInfo1.inverseMass * deltaTime;
        velocity2.linear -= direction * force * massInfo2.inverseMass * deltaTime;
    }
#else
#error Not implemented! //TODO
#endif
}

#include "MoveSystem.h"

ComponentsBitset MoveSystem::createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Location));
    bitset.set(static_cast<size_t>(ComponentType::Velocity));
    bitset.set(static_cast<size_t>(ComponentType::Acceleration));
    return bitset;
}

MoveSystem::MoveSystem() : System(createCurrentSystemBitset())
{ }

void MoveSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &location = context.getComponent<LocationComponent>(id);
    auto &velocity = context.getComponent<VelocityComponent>(id);
    auto &acceleration = context.getComponent<AccelerationComponent>(id);
#ifdef USE_EULER
    location += velocity * deltaTime;
    velocity += acceleration * deltaTime;
#elif defined(USE_SYMPLECTIC_EULER)
    velocity += acceleration * deltaTime;
    location += velocity * deltaTime;
#elif defined(USE_RK4)
    real halfStep = deltaTime / 2;
    Transform k1 = velocity;
    Transform k2 = velocity + acceleration * halfStep;
    Transform k3 = velocity + acceleration * halfStep;
    Transform k4 = velocity + acceleration * deltaTime;

    location += (k1 + (k2 + k3) * 2 + k4) * (deltaTime / 6);
    velocity += acceleration * deltaTime;
#elif defined(USE_LEAPFROG)
    real halfStep = deltaTime / 2;
    velocity += acceleration * halfStep;
    location += velocity * deltaTime;
    velocity += acceleration * halfStep;
#else
#error No moving algorithm is defined!
#endif
}

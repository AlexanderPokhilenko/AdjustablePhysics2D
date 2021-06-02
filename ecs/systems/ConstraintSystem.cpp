#include "ConstraintSystem.h"
#include "../../math/MathExtensions.h"

ComponentsBitset ConstraintSystem::createCurrentSystemBitset() {
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Constraint));
    bitset.set(static_cast<size_t>(ComponentType::Velocity));
#ifdef USE_ACCELERATION
    bitset.set(static_cast<size_t>(ComponentType::Acceleration));
#endif
    return bitset;
}

ConstraintSystem::ConstraintSystem() : System(createCurrentSystemBitset())
{ }

void ConstraintSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &constraint = context.getComponent<ConstraintComponent>(id);
    auto &velocity = context.getComponent<VelocityComponent>(id);

    auto axisMoving = constraint.axis.x != 0 && constraint.axis.y != 0;
    if(axisMoving) velocity.linear = constraint.axis * Vector2::dotProduct(velocity.linear, constraint.axis);
    if(velocity.linear.getSqrMagnitude() > constraint.maxLinearVelocity * constraint.maxLinearVelocity)
        velocity.linear = velocity.linear.getNormalized() * constraint.maxLinearVelocity;
#ifdef USE_ROTATION
    if((constraint.rotation == Constraint::RotationConstraint::OnlyClockwise && velocity.angular < 0) ||
       (constraint.rotation == Constraint::RotationConstraint::OnlyCounterclockwise && velocity.angular > 0)) {
        velocity.angular = 0;
    } else {
        auto absAngularVelocity = fabsr(velocity.angular);
        if(absAngularVelocity > constraint.maxAngularVelocity) velocity.angular *= constraint.maxAngularVelocity / absAngularVelocity;
    }
#endif
#ifdef USE_ACCELERATION
    auto &acceleration = context.getComponent<AccelerationComponent>(id);
    if(axisMoving) acceleration.linear = constraint.axis * Vector2::dotProduct(acceleration.linear, constraint.axis);
    if(acceleration.linear.getSqrMagnitude() > constraint.maxLinearAcceleration * constraint.maxLinearAcceleration)
        acceleration.linear = acceleration.linear.getNormalized() * constraint.maxLinearAcceleration;
#ifdef USE_ROTATION
    if((constraint.rotation == Constraint::RotationConstraint::OnlyClockwise && acceleration.angular < 0) ||
       (constraint.rotation == Constraint::RotationConstraint::OnlyCounterclockwise && acceleration.angular > 0)) {
        acceleration.angular = 0;
    } else {
        auto absAngularAcceleration = fabsr(acceleration.angular);
        if(absAngularAcceleration > constraint.maxAngularAcceleration) acceleration.angular *= constraint.maxAngularAcceleration / absAngularAcceleration;
    }
#endif
#endif
}

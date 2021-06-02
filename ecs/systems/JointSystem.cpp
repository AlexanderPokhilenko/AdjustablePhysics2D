#include "JointSystem.h"

JointSystem::JointSystem() : System() { }

void JointSystem::update(Context &context, EntityId id, real deltaTime) { }

void JointSystem::update(Context &context, real deltaTime) {
#ifndef USE_JOINT
    System::update(context, deltaTime);
#else
    context.joints.forEach([&context, deltaTime](Joint &joint) {
        auto id1 = joint.id1;
        auto id2 = joint.id2;
        auto &location1 = context.getComponent<LocationComponent>(id1);
        auto &location2 = context.getComponent<LocationComponent>(id2);
        auto position1 = location1.linear;
        auto position2 = location2.linear;
#ifdef USE_ROTATION
        auto point1 = joint.point1.getRotated(location1.angular);
        auto point2 = joint.point2.getRotated(location2.angular);
#else
        auto point1 = joint.point1;
        auto point2 = joint.point2;
#endif
        position1 += point1;
        position2 += point2;
        auto displacement = position2 - position1;
        auto magnitude = displacement.getMagnitude();
        auto direction = displacement / magnitude;
        Vector2 relativeVelocityVector{};
        auto hasVelocity1 = context.hasComponent<VelocityComponent>(id1);
        auto &velocity1 = context.getComponent<VelocityComponent>(id1);
        if(hasVelocity1) {
            relativeVelocityVector -= velocity1.linear;
#ifdef USE_ROTATION
            relativeVelocityVector -= -Vector2{point1.y, -point1.x} * velocity1.angular;
#endif
        }
        auto hasVelocity2 = context.hasComponent<VelocityComponent>(id2);
        auto &velocity2 = context.getComponent<VelocityComponent>(id2);
        if(hasVelocity2) {
            relativeVelocityVector += velocity2.linear;
#ifdef USE_ROTATION
            relativeVelocityVector += -Vector2{point2.y, -point2.x} * velocity2.angular;
#endif
        }
        auto deltaLength = magnitude - joint.length;
        auto relativeVelocity = Vector2::dotProduct(relativeVelocityVector, direction);
        auto force = deltaLength * joint.kSpring + relativeVelocity * joint.kDamper;
        auto impulseVector = direction * (force * deltaTime);

        auto hasMass1 = context.hasComponent<MassInfoComponent>(id1);
        auto hasMass2 = context.hasComponent<MassInfoComponent>(id2);
        auto &massInfo1 = context.getComponent<MassInfoComponent>(id1);
        auto &massInfo2 = context.getComponent<MassInfoComponent>(id2);

        if(hasVelocity1 && hasMass1) velocity1 += impulseVector * massInfo1.inverseMass;
        if(hasVelocity2 && hasMass2) velocity2 -= impulseVector * massInfo2.inverseMass;
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        if(hasVelocity1 && hasMass1 && context.getComponent<ShapeComponent>(id1).shapeType != ShapeType::AABB)
            velocity1.angular += massInfo1.inverseInertia * Vector2::crossProduct(point1, impulseVector);
        if(hasVelocity2 && hasMass2 && context.getComponent<ShapeComponent>(id2).shapeType != ShapeType::AABB)
            velocity2.angular -= massInfo2.inverseInertia * Vector2::crossProduct(point2, impulseVector);
#endif
    });
#endif
}

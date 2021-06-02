#ifndef ADJUSTABLEPHYSICS2D_CONSTRAINT_H
#define ADJUSTABLEPHYSICS2D_CONSTRAINT_H

#include "../math/Vector2.h"
#include "../math/MathExtensions.h"

struct Constraint {
    Vector2 axis;
    real maxLinearVelocity;
#ifdef USE_ACCELERATION
    real maxLinearAcceleration;
#endif
#ifdef USE_ROTATION
    enum class RotationConstraint {
        AnyDirection,
        OnlyClockwise,
        OnlyCounterclockwise
    };
    RotationConstraint rotation;
    real maxAngularVelocity;
#ifdef USE_ACCELERATION
    real maxAngularAcceleration;
#endif
#endif

#ifdef USE_ACCELERATION
#ifdef USE_ROTATION
    Constraint(Vector2 axis = Vector2{0, 0}, real maxLinearVelocity = REAL_MAX, real maxLinearAcceleration = REAL_MAX, RotationConstraint rotation = RotationConstraint::AnyDirection, real maxAngularVelocity = REAL_MAX, real maxAngularAcceleration = REAL_MAX) :
            axis(axis), maxLinearVelocity(maxLinearVelocity), maxLinearAcceleration(maxLinearAcceleration), rotation(rotation), maxAngularVelocity(maxAngularVelocity), maxAngularAcceleration(maxAngularAcceleration) { };
#else
    Constraint(Vector2 axis = Vector2{0, 0}, real maxLinearVelocity = REAL_MAX, real maxLinearAcceleration = REAL_MAX) :
            axis(axis), maxLinearVelocity(maxLinearVelocity), maxLinearAcceleration(maxLinearAcceleration) { };
#endif
#else
#ifdef USE_ROTATION
    Constraint(Vector2 axis = Vector2{0, 0}, real maxLinearVelocity = REAL_MAX, RotationConstraint rotation = RotationConstraint::AnyDirection, real maxAngularVelocity = REAL_MAX) :
            axis(axis), maxLinearVelocity(maxLinearVelocity), rotation(rotation), maxAngularVelocity(maxAngularVelocity) { };
#else
    Constraint(Vector2 axis = Vector2{0, 0}, real maxLinearVelocity = REAL_MAX) :
            axis(axis), maxLinearVelocity(maxLinearVelocity) { };
#endif
#endif
};

#endif //ADJUSTABLEPHYSICS2D_CONSTRAINT_H

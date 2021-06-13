#ifdef USE_CONSTRAINT
#include <cmath>
#include "gtest/gtest.h"
#include "../src/ecs/Context.h"
#include "../src/ecs/systems/ConstraintSystem.h"

TEST(ConstraintSystemTest, NoMoveEntities) {
    Context context(3, 0);

    const auto count = 2;
    EntityId ids[count] = {context.createEntity(), context.createEntity()};

    Vector2 velocity{1, 1};

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id);
        context.addComponent<VelocityComponent>(id, velocity);
#ifdef USE_ACCELERATION
        context.addComponent<AccelerationComponent>(id);
#endif
    }
    Constraint c1, c2;

    c1.maxLinearVelocity = 0;
    c2.axis = {-1, 1};

    context.addComponent<ConstraintComponent>(ids[0], c1);
    context.addComponent<ConstraintComponent>(ids[1], c2);

    System *system = new ConstraintSystem();
    system->update(context, 1);

    for(auto id : ids) ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<real>,
            context.getComponent<VelocityComponent>(id).linear.getSqrMagnitude(), 0);

    delete system;
}

TEST(ConstraintSystemTest, MaxMovingEntities) {
    Context context(3, 0);

    const auto count = 16;
    EntityId ids[count];

    for(auto &id : ids) id = context.createEntity();

    Vector2 vector{5, 0};

    Constraint c;
    c.maxLinearVelocity = 1;
    auto mlv_sqr = c.maxLinearVelocity * c.maxLinearVelocity;
#ifdef USE_ACCELERATION
    c.maxLinearAcceleration = 2;
    auto mla_sqr = c.maxLinearAcceleration * c.maxLinearAcceleration;
#endif

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id);
        auto currentVector = vector.getRotated(real(2 * M_PI * id) / real(count));
        context.addComponent<VelocityComponent>(id, currentVector);
#ifdef USE_ACCELERATION
        context.addComponent<AccelerationComponent>(id, currentVector);
#endif
        context.addComponent<ConstraintComponent>(id, c);
    }

    System *system = new ConstraintSystem();
    system->update(context, 1);

    for (auto id : ids) {
        ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<real>,
                            context.getComponent<VelocityComponent>(id).linear.getSqrMagnitude(), mlv_sqr);
#ifdef USE_ACCELERATION
        ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<real>,
                            context.getComponent<AccelerationComponent>(id).linear.getSqrMagnitude(), mla_sqr);
#endif
    }

    delete system;
}

#endif

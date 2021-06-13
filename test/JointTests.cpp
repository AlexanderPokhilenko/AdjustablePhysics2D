#ifdef USE_JOINT
#include "gtest/gtest.h"
#include "../src/ecs/Context.h"
#include "../src/ecs/systems/JointSystem.h"

TEST(JointSystemTest, MoveEntities) {
    Context context(3, 0);

    const auto count = 2;
    EntityId ids[count] = {context.createEntity(), context.createEntity()};

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id, Vector2{real(id), real((id + 1) % count)});
        context.addComponent<VelocityComponent>(id);
        context.addComponent<MassInfoComponent>(id);
#ifdef USE_ACCELERATION
        context.addComponent<AccelerationComponent>(id);
#endif
    }

    context.joints.add(ids[0], ids[1], ids[0], ids[1], 1000, 10, 0.5);

    System *system = new JointSystem();
    system->update(context, 1);

    ASSERT_TRUE(context.getComponent<VelocityComponent>(ids[0]).linear.getSqrMagnitude() > 0);
    EXPECT_EQ(context.getComponent<VelocityComponent>(ids[0]).linear, -context.getComponent<VelocityComponent>(ids[1]).linear);

    delete system;
}

TEST(JointSystemTest, NoMoveFirstEntity) {
    Context context(3, 0);

    const auto count = 2;
    EntityId ids[count] = {context.createEntity(), context.createEntity()};

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id, Vector2{real(id), real((id + 1) % count)});
        context.addComponent<VelocityComponent>(id);
        context.addComponent<MassInfoComponent>(id);
#ifdef USE_ACCELERATION
        context.addComponent<AccelerationComponent>(id);
#endif
    }

    context.removeComponent<VelocityComponent>(ids[0]);

    context.joints.add(ids[0], ids[1], ids[0], ids[1], 1000, 10, 0.5);

    System *system = new JointSystem();
    system->update(context, 1);

    ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<real>, context.getComponent<VelocityComponent>(ids[0]).linear.getSqrMagnitude(), 0);

    ASSERT_TRUE(context.getComponent<VelocityComponent>(ids[1]).linear.getSqrMagnitude() > 0);

    delete system;
}
#endif

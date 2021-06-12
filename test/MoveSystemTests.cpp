#include "gtest/gtest.h"
#include "../src/ecs/Context.h"
#include "../src/ecs/systems/MoveSystem.h"

TEST(MoveSystemTest, MoveEntities) {
    Context context(3, 0);

    const auto count = 3;
    EntityId ids[count] = {context.createEntity(), context.createEntity(), context.createEntity()};

    Vector2 velocity{1, 1};

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id);
        context.addComponent<VelocityComponent>(id, velocity);
#ifdef USE_ACCELERATION
        context.addComponent<AccelerationComponent>(id);
#endif
    }

    System *system = new MoveSystem();
    system->update(context, 1);

    for(auto id : ids) EXPECT_EQ(context.getComponent<LocationComponent>(id).linear, velocity);

    delete system;
}

TEST(MoveSystemTest, NoMoveEntities) {
    Context context(3, 0);

    const auto count = 3;
    EntityId ids[count] = {context.createEntity(), context.createEntity(), context.createEntity()};

    Vector2 position{1, 1};

    for(auto id : ids) context.addComponent<LocationComponent>(id, position);

    System *system = new MoveSystem();
    system->update(context, 1);

    for(auto id : ids) EXPECT_EQ(context.getComponent<LocationComponent>(id).linear, position);

    delete system;
}

TEST(MoveSystemTest, RemovedNoMoveEntities) {
    Context context(3, 0);

    const auto count = 3;
    EntityId ids[count] = {context.createEntity(), context.createEntity(), context.createEntity()};

    Vector2 location{1, 1};

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id, location);
        context.addComponent<VelocityComponent>(id);
        context.removeComponent<VelocityComponent>(id);
#ifdef USE_ACCELERATION
        context.addComponent<AccelerationComponent>(id);
        context.removeComponent<AccelerationComponent>(id);
#endif
    }

    System *system = new MoveSystem();
    system->update(context, 1);

    for(auto id : ids) EXPECT_EQ(context.getComponent<LocationComponent>(id).linear, location);

    delete system;
}

#ifdef USE_ACCELERATION
TEST(MoveSystemTest, MoveWithAccelerationEntities) {
    Context context(1, 0);

    auto id = context.createEntity();

    context.addComponent<LocationComponent>(id);
    context.addComponent<VelocityComponent>(id);

#ifdef USE_ROTATION
    context.addComponent<AccelerationComponent>(id, Vector2{1, 1}, 1);
#else
    context.addComponent<AccelerationComponent>(id, Vector2{1, 1});
#endif

    System *system = new MoveSystem();
    system->update(context, 1);

    EXPECT_TRUE(context.getComponent<LocationComponent>(id).linear.getSqrMagnitude() != 0);
    EXPECT_TRUE(context.getComponent<VelocityComponent>(id).linear.getSqrMagnitude() != 0);
#ifdef USE_ROTATION
    EXPECT_TRUE(context.getComponent<LocationComponent>(id).angular != 0);
    EXPECT_TRUE(context.getComponent<VelocityComponent>(id).angular != 0);
#endif

    delete system;
}
#endif

#include "gtest/gtest.h"
#include "../src/ecs/Context.h"

#ifdef USE_GLOBAL_GRAVITATION
#include "../src/ecs/systems/GlobalGravitationSystem.h"

TEST(GlobalGravitationSystemTest, ChangeVelocity) {
    Context context(1, 0);

    auto id = context.createEntity();

    context.addComponent<VelocityComponent>(id);
    context.addComponent<MassInfoComponent>(id);

    auto *gravitationSystem = new GlobalGravitationSystem();
    gravitationSystem->g = {0, 1};
    System *system = gravitationSystem;
    system->update(context, 1);

    EXPECT_EQ(context.getComponent<VelocityComponent>(id).linear, gravitationSystem->g);

    delete system;
}

TEST(GlobalGravitationSystemTest, NoChangeVelocity) {
    Context context(1, 0);

    auto id = context.createEntity();
    Vector2 velocity = {1, 1};

    context.addComponent<VelocityComponent>(id, velocity);

    System *system = new GlobalGravitationSystem();

    system->update(context, 1);

    EXPECT_EQ(context.getComponent<VelocityComponent>(id).linear, velocity);

    delete system;
}
#endif

#ifdef USE_GRAVITATIONAL_FORCE
#include "../src/ecs/systems/GravitationalForceSystem.h"

TEST(GravitationalForceSystemTest, ChangeVelocity) {
    Context context(2, 0);

    auto id1 = context.createEntity();
    auto id2 = context.createEntity();

    context.addComponent<LocationComponent>(id1, Vector2{-1, -1});
    context.addComponent<LocationComponent>(id2, Vector2{1, 1});
    context.addComponent<VelocityComponent>(id1);
    context.addComponent<VelocityComponent>(id2);
    context.addComponent<MassInfoComponent>(id1);
    context.addComponent<MassInfoComponent>(id2);

    System *system = new GravitationalForceSystem();

    system->update(context, 1);

    ASSERT_TRUE(context.getComponent<VelocityComponent>(id1).linear.getSqrMagnitude() > 0);
    EXPECT_EQ(context.getComponent<VelocityComponent>(id1).linear, -context.getComponent<VelocityComponent>(id2).linear);

    delete system;
}

#ifndef USE_EQUAL_MASSES
TEST(GravitationalForceSystemTest, ChangeVelocityDifferentMass) {
    Context context(2, 0);

    auto id1 = context.createEntity();
    auto id2 = context.createEntity();

    context.addComponent<LocationComponent>(id1, Vector2{-1, -1});
    context.addComponent<LocationComponent>(id2, Vector2{1, 1});
    context.addComponent<VelocityComponent>(id1);
    context.addComponent<VelocityComponent>(id2);
    MassInfo m1, m2;
    m1.mass = 1;
    m1.inverseMass = 1 / m1.mass;
    m2.mass = 2;
    m2.inverseMass = 1 / m2.mass;
    context.addComponent<MassInfoComponent>(id1, m1);
    context.addComponent<MassInfoComponent>(id2, m2);

    auto *gravitationSystem = new GravitationalForceSystem();
    gravitationSystem->G = 1;
    System *system = gravitationSystem;

    system->update(context, 1);

    EXPECT_TRUE(Vector2::almostEquals(context.getComponent<VelocityComponent>(id1).linear * m2.inverseMass * m1.mass, -context.getComponent<VelocityComponent>(id2).linear));

    delete system;
}
#endif

TEST(GravitationalForceSystemTest, NoChangeVelocity) {
    Context context(2, 0);

    auto id1 = context.createEntity();
    auto id2 = context.createEntity();

    context.addComponent<LocationComponent>(id1, Vector2{-1, -1});
    context.addComponent<LocationComponent>(id2, Vector2{1, 1});
    context.addComponent<VelocityComponent>(id1);
    context.addComponent<VelocityComponent>(id2);
    context.addComponent<MassInfoComponent>(id1);

    System *system = new GravitationalForceSystem();

    system->update(context, 1);

    EXPECT_EQ(context.getComponent<VelocityComponent>(id1).linear.getSqrMagnitude(), 0);
    EXPECT_EQ(context.getComponent<VelocityComponent>(id2).linear.getSqrMagnitude(), 0);

    delete system;
}
#endif

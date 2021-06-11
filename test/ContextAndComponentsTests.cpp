#include "gtest/gtest.h"
#include "../src/PhysicsEngine.h"

#ifndef USE_AABB_ONLY
TEST(ComponentTest, ShapeWithRadius) {
    PhysicsEngine engine;
    real radius = 5;
    auto entity = engine.createCircle(radius, {{0, 0}});
    ASSERT_EQ(0, entity.id);
    ASSERT_TRUE(entity.hasComponent(ComponentType::Shape));
    EXPECT_EQ(radius, entity.getComponent<ShapeComponent>().radius);
}
#endif

TEST(ComponentContainerTest, AddComponent) {
    ComponentContainer<LocationComponent> container(1);
    EntityId id = 0;
    LocationComponent l1 {{real(1), real(2)}};
#ifdef USE_ROTATION
    l1.angular = real(3);
#endif
    container.add(id, l1);

    const auto &l2 = container.get(id);
    EXPECT_EQ(l2.linear, l1.linear);
#ifdef USE_ROTATION
    EXPECT_EQ(l2.angular, l1.angular);
#endif
}

TEST(ContextTest, CreateEntity) {
    Context context(2, 0);
    auto id1 = context.createEntity();
    auto id2 = context.createEntity();
    EXPECT_TRUE(id2 > id1);
    EXPECT_TRUE(context.hasEntity(id1));
    EXPECT_TRUE(context.hasEntity(id2));
    EXPECT_EQ(context.getEntitiesSize(), 2);
}

TEST(ContextTest, HasEntity) {
    Context context(1, 1);
    auto id = context.createEntity();
    EXPECT_TRUE(context.hasEntity(id));
    context.deleteEntity(id);
    EXPECT_FALSE(context.hasEntity(id));
}

TEST(ContextTest, RecreateEntityLast) {
    Context context(2, 1);
    context.createEntity();
    auto id2Old = context.createEntity();
    context.deleteEntity(id2Old);
    auto id2New = context.createEntity();
    EXPECT_EQ(id2New, id2Old);
}

TEST(ContextTest, RecreateEntityMiddle) {
    Context context(2, 1);
    auto id1Old = context.createEntity();
    context.createEntity();
    context.deleteEntity(id1Old);
    auto id1New = context.createEntity();
    EXPECT_EQ(id1New, id1Old);
}

TEST(ContextTest, ComponentCRUD) {
    Context context(1, 0);
    auto id = context.createEntity();

    for(int i = 0; i < ComponentsCount; i++) EXPECT_FALSE(context.hasComponent(id, ComponentType(i)));

    LocationComponent l1 {{1, 2}};
    context.addComponent<LocationComponent>(id, l1);
    ASSERT_TRUE(context.hasComponent<LocationComponent>(id));

    auto &l2 = context.getComponent<LocationComponent>(id);
    EXPECT_EQ(l2.linear, l1.linear);
#ifdef USE_ROTATION
    EXPECT_EQ(l2.angular, l1.angular);
#endif

    Vector2 newLinear{-1, -2};
    l2.linear = newLinear;
    EXPECT_EQ(context.getComponent<LocationComponent>(id).linear, newLinear);

    context.removeComponent<LocationComponent>(id);
    EXPECT_FALSE(context.hasComponent<LocationComponent>(id));
}

TEST(ContextTest, ComponentsCheck) {
    Context context(1, 0);
    auto id = context.createEntity();

    context.addComponent<LocationComponent>(id);
    context.addComponent<VelocityComponent>(id);

    ASSERT_TRUE(context.hasComponent<LocationComponent>(id));
    ASSERT_TRUE(context.hasComponent<VelocityComponent>(id));

    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Location));
    bitset.set(static_cast<size_t>(ComponentType::Velocity));

    ASSERT_TRUE(context.checkEntity(id, bitset));
}

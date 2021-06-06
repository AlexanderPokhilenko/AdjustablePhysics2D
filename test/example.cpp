#include "gtest/gtest.h"
#include "../src/PhysicsEngine.h"

TEST(ComponentTest, ShapeWithRadius) {
    PhysicsEngine engine;
    real radius = 5;
    auto entity = engine.createCircle(radius, {{0, 0}});
    ASSERT_EQ(0, entity.id);
    ASSERT_TRUE(entity.hasComponent(ComponentType::Shape));
    EXPECT_EQ(radius, entity.getComponent<ShapeComponent>().radius);
}

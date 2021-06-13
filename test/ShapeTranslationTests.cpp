#include "gtest/gtest.h"
#include "../src/ecs/Context.h"
#include "../src/ecs/systems/ShapeTranslationSystem.h"
#include "../src/math/MathExtensions.h"

TEST(ShapeTranslationSystemTest, MoveShapes) {
    Context context(3, 0);

    const auto count = ShapesCount;

    EntityId ids[count];
    for(auto &id : ids) id = context.createEntity();

    Vector2 position{1, 1};

    for(auto id : ids) {
        context.addComponent<LocationComponent>(id, position);
        Shape s;
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
        s.shapeType = ShapeType(id);
#ifndef USE_PRIMITIVES_ONLY
        if(s.shapeType == ShapeType::Complex) {
            auto verticesCount = 4;
            auto vertices = new Vector2[verticesCount] {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
            auto normals = new Vector2[verticesCount] {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
            context.addComponent<PolygonComponent>(id, verticesCount, vertices, normals);
        }
#endif
#endif
#ifndef USE_CIRCLES_ONLY
        s.boundingBox = {{-1, -1}, {1, 1}};
#endif
#ifndef USE_AABB_ONLY
        s.centroid = {0, 0};
        s.radius = sqrtr(2);
#endif

        context.addComponent<ShapeComponent>(id, s);
    }

    System *system = new ShapeTranslationSystem();
    system->update(context, 1);

    for(auto id : ids) {
        const auto &shape = context.getComponent<ShapeComponent>(id);
        EXPECT_EQ(shape.getCenter(), position);
#ifndef USE_CIRCLES_ONLY
        EXPECT_EQ(shape.boundingBox.getCenter(), position);
#endif
#ifndef USE_AABB_ONLY
        EXPECT_EQ(shape.centroid, position);
#endif
    }

    delete system;
}

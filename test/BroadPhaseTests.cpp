#ifdef USE_BROAD_PHASE
#include "gtest/gtest.h"
#include "../src/ecs/Context.h"
#include "../src/math/MathExtensions.h"
#include "../src/ecs/systems/BroadPhaseSystem.h"

TEST(BroadPhaseSystemTest, PotentialCollisions) {
    class TestableContext : public Context {
        using Context::Context;
    public:
        bool containsPossibleCollision(EntityId id1, EntityId id2) {
            for (const auto &pair : possibleCollisions) if(pair.first == id1 && pair.second == id2) return true;
            return false;
        }
    };
    TestableContext context(3, 0);

    EntityId ids1[ShapesCount][ShapesCount], ids2[ShapesCount][ShapesCount];

    for (int i = 0; i < ShapesCount; ++i) {
        for (int j = 0; j < ShapesCount; ++j) {
            auto id1 = context.createEntity();
            ids1[i][j] = id1;
            auto id2 = context.createEntity();
            ids2[i][j] = id2;

            Vector2 position1 = {real(10 * i), real(10 * j)}, position2 = position1 + Vector2{real(0.5), real(0.5)};
            context.addComponent<LocationComponent>(id1, position1);
            context.addComponent<LocationComponent>(id2, position2);

            Shape s1, s2;

#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
            s1.shapeType = ShapeType(i);
            s2.shapeType = ShapeType(j);
#ifndef USE_PRIMITIVES_ONLY
            if(s1.shapeType == ShapeType::Complex || s2.shapeType == ShapeType::Complex) {
                const auto verticesCount = 4;
                const auto vertices = new Vector2[verticesCount] {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
                const auto normals = new Vector2[verticesCount] {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
                if(s1.shapeType == ShapeType::Complex) context.addComponent<PolygonComponent>(id1, verticesCount, vertices, normals);
                if(s2.shapeType == ShapeType::Complex) context.addComponent<PolygonComponent>(id2, verticesCount, vertices, normals);
            }
#endif
#endif

#ifndef USE_CIRCLES_ONLY
            s1.boundingBox = AABB{{-1, -1}, {1, 1}} + position1;
            s2.boundingBox = AABB{{-1, -1}, {1, 1}} + position2;
#endif
#ifndef USE_AABB_ONLY
            s1.centroid = position1;
            s2.centroid = position2;
            s1.radius = sqrtr(2);
            s2.radius = sqrtr(2);
#endif
            context.addComponent<ShapeComponent>(id1, s1);
            context.addComponent<ShapeComponent>(id2, s2);
        }
    }

#ifdef USE_QUADTREE
    auto *quadtree = new Quadtree({{-100, -100}, {100, 100}});
    System *system = new BroadPhaseSystem(quadtree);
#else
    System *system = new BroadPhaseSystem();
#endif
    system->update(context, 1);

    for (int i = 0; i < ShapesCount; ++i) {
        for (int j = 0; j < ShapesCount; ++j) {
            EXPECT_TRUE(context.containsPossibleCollision(ids1[i][j], ids2[i][j]));
        }
    }

    delete system;
}

#endif

#include "gtest/gtest.h"
#include "../src/ecs/Context.h"
#include <algorithm>
#ifdef USE_QUADTREE
#include "../src/common/Quadtree.h"

TEST(QuadtreeTest, AddPoint) {
    AABB bounds{{-50, -50}, {50,  50}};
    Quadtree quadtree(bounds);

    Context context;
    auto e1 = context.createEntity();
    Vector2 position{0, 0};
    context.addComponent<LocationComponent>(e1, position);

    quadtree.addPoint(e1, context);
    quadtree.forEachLeaf([e1](const std::vector<EntityId> &vec) {
        auto it = std::find(vec.cbegin(), vec.cend(), e1);
        ASSERT_FALSE(it == vec.cend());
    });
}

TEST(QuadtreeTest, RemovePoint) {
    AABB bounds{{-50, -50}, {50,  50}};
    Quadtree quadtree(bounds);

    Context context;
    for(int i = 0; i < 8; i++) {
        auto e = context.createEntity();
        Vector2 position{0, 0};
        context.addComponent<LocationComponent>(e, position);

        quadtree.addPoint(e, context);
        quadtree.removePoint(e);
    }
    quadtree.forEachLeaf([](const std::vector<EntityId> &vec) {
        ASSERT_EQ(vec.size(), 0);
    });
}

TEST(QuadtreeTest, AddPointAndSplit) {
    AABB bounds{{-50, -50}, {50,  50}};
    auto center = bounds.getCenter(), halfSize = bounds.getHalfSize(), quarterSize = halfSize * 0.5;
    Quadtree quadtree(bounds, 8);

    Context context;
    std::array<EntityId, 21> ids{};
    for(int i = 0; i < 4; i++) {
        real signY = i < 2 ? +1 : -1;
        real signX = i % 2 == 0 ? signY : -signY;

        auto position = center + Vector2{signX * quarterSize.x, signY * quarterSize.y};

        auto e1 = context.createEntity();
        context.addComponent<LocationComponent>(e1, position);
        quadtree.addPoint(e1, context);
        ids[i * 4] = e1;

        auto e2 = context.createEntity();
        context.addComponent<LocationComponent>(e2, position + Vector2{signX * halfSize.x, 0});
        quadtree.addPoint(e2, context);
        ids[i * 4 + 1] = e2;

        auto e3 = context.createEntity();
        context.addComponent<LocationComponent>(e3, position + Vector2{0, signY * halfSize.y});
        quadtree.addPoint(e3, context);
        ids[i * 4 + 2] = e3;

        auto e4 = context.createEntity();
        context.addComponent<LocationComponent>(e4, position + Vector2{signX * halfSize.x, signY * halfSize.y});
        quadtree.addPoint(e4, context);
        ids[i * 4 + 3] = e4;
    }

    auto em1 = context.createEntity();
    context.addComponent<LocationComponent>(em1, Vector2{0, quarterSize.y});
    quadtree.addPoint(em1, context);
    ids[16] = em1;

    auto em2 = context.createEntity();
    context.addComponent<LocationComponent>(em2, Vector2{-quarterSize.x, 0});
    quadtree.addPoint(em2, context);
    ids[17] = em2;

    auto em3 = context.createEntity();
    context.addComponent<LocationComponent>(em3, Vector2{0, -quarterSize.y});
    quadtree.addPoint(em3, context);
    ids[18] = em3;

    auto em4 = context.createEntity();
    context.addComponent<LocationComponent>(em4, Vector2{quarterSize.x, 0});
    quadtree.addPoint(em4, context);
    ids[19] = em4;

    auto em5 = context.createEntity();
    context.addComponent<LocationComponent>(em5, Vector2{0, 0});
    quadtree.addPoint(em5, context);
    ids[20] = em5;

    std::vector<EntityId> all;
    all.reserve(ids.size());

    std::vector<std::vector<EntityId>> vectors;
    vectors.reserve(4);

    quadtree.forEachLeaf([&all, &vectors](const std::vector<EntityId> &vec) {
        vectors.push_back(vec);
        all.insert(all.end(), vec.cbegin(), vec.cend());
    });

    EXPECT_EQ(all.size(), ids.size());
    EXPECT_EQ(vectors.size(), 4);

    for(int i = 0; i < 4; i++) {
        const auto& vec = vectors[i];
        for(int j = 0; j < 4; j++) {
            EXPECT_EQ(vec[j], ids[i * 4 + j]);
        }
    }

    EXPECT_EQ(vectors[1][4], ids[16]);
    EXPECT_EQ(vectors[2][4], ids[17]);
    EXPECT_EQ(vectors[2][5], ids[18]);
    EXPECT_EQ(vectors[3][4], ids[19]);
    EXPECT_EQ(vectors[2][6], ids[20]);
}

TEST(QuadtreeTest, AddShapeAndSplit) {
    AABB bounds{{-50, -50}, {50,  50}};
    auto center = bounds.getCenter(), halfSize = bounds.getHalfSize(), quarterSize = halfSize * 0.5;
    Quadtree quadtree(bounds, 8);

    Context context;
    std::array<EntityId, 9> ids{};

    auto addShape = [&context, &quadtree](EntityId id, Vector2 p1, Vector2 p2) {
        if(p2.x < p1.x) std::swap(p1.x, p2.x);
        if(p2.y < p1.y) std::swap(p1.y, p2.y);
        AABB aabb = {p1, p2};
        Shape shape{};
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
        shape.shapeType = ShapeType::AABB;
#endif
#ifndef USE_CIRCLES_ONLY
        shape.boundingBox = aabb;
#endif
#ifndef USE_AABB_ONLY
        shape.centroid = aabb.getCenter();
        auto halfSize1 = aabb.getHalfSize();
        shape.radius = halfSize1.getMagnitude();
#endif
        context.addComponent<ShapeComponent>(id, shape);
        quadtree.addShape(id, context);
    };

    for(int i = 0; i < 4; i++) {
        real signY = i < 2 ? +1 : -1;
        real signX = i % 2 == 0 ? signY : -signY;

        auto position1 = center + Vector2{signX * quarterSize.x, signY * quarterSize.y};

        auto e1 = context.createEntity();
        addShape(e1, center, position1);
        ids[i * 2] = e1;

        auto position2 = center + Vector2{signX * halfSize.x, signY * halfSize.y};

        auto e2 = context.createEntity();
        addShape(e2, position1, position2);
        ids[i * 2 + 1] = e2;
    }

    auto ec = context.createEntity();
    addShape(ec, center - quarterSize, center + quarterSize);
    ids[8] = ec;

    std::vector<EntityId> all;
    all.reserve(ids.size());

    std::vector<std::vector<EntityId>> vectors;
    vectors.reserve(4);

    quadtree.forEachLeaf([&all, &vectors](const std::vector<EntityId> &vec) {
        vectors.push_back(vec);
        all.insert(all.end(), vec.cbegin(), vec.cend());
    });

    EXPECT_EQ(vectors.size(), 4);

    for(int i = 0; i < 4; i++) {
        const auto& vec = vectors[i];
        EXPECT_EQ(vec[i + 1], ids[i * 2 + 1]);
        for(int j = 0; j <= i; j++) EXPECT_EQ(vec[j], ids[j * 2]);
        for(int j = i + 1; j < 4; j++) EXPECT_EQ(vec[j + 1], ids[j * 2]);
        EXPECT_EQ(vec.back(), ids[8]);
    }
}

TEST(QuadtreeTest, RemoveShape) {
    AABB bounds{{-50, -50}, {50,  50}};
    Quadtree quadtree(bounds, 1);

    Context context;
    for(int i = 0; i < 4; i++) {
        auto e = context.createEntity();
        real signY = i < 2 ? +1 : -1;
        real signX = i % 2 == 0 ? signY : -signY;

        Vector2 position{ signX * 10, signY * 10};

        Shape shape{};
        auto min = position - Vector2{5, 5}, max = position + Vector2{5, 5};
        AABB aabb {min, max};
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
        shape.shapeType = ShapeType::AABB;
#endif
#ifndef USE_CIRCLES_ONLY
        shape.boundingBox = aabb;
#endif
#ifndef USE_AABB_ONLY
        shape.centroid = aabb.getCenter();
        auto halfSize1 = aabb.getHalfSize();
        shape.radius = halfSize1.getMagnitude();
#endif
        context.addComponent<ShapeComponent>(e, shape);
        quadtree.addShape(e, context);
        quadtree.removeShape(e);
    }
    quadtree.forEachLeaf([](const std::vector<EntityId> &vec) {
        ASSERT_EQ(vec.size(), 0);
    });
}
#endif

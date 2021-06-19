#ifndef ADJUSTABLEPHYSICS2D_QUADTREE_H
#define ADJUSTABLEPHYSICS2D_QUADTREE_H

#include <vector>
#include <functional>
#include "AABB.h"
#include "../base/primitives.h"
#include "../ecs/Context.h"

class Quadtree {
private:
    const size_t Threshold;
    const size_t MaxDepth;
    struct Node {
        const AABB bounds;
        Node *children[4];
        std::vector<EntityId> values;
#ifdef USE_CIRCLES_ONLY
        const real radius;
        const Vector2 center;
#endif
    private:
        enum class Direction { NorthEast, NorthWest, SouthWest, SouthEast };
        inline void add(EntityId id) { values.push_back(id); }
    public:
        explicit Node(AABB bounds, size_t capacity = 0);
        inline bool isLeaf() const { return children[0] == nullptr; }
        void createChildren();
        void addShape(EntityId id, const Shape& shape, const Context& context, size_t threshold, size_t maxDepth, size_t depth = 0);
        void addPoint(EntityId id, const Vector2& position, const Context& context, size_t threshold, size_t maxDepth, size_t depth = 0);
        bool removeShape(EntityId id);
        bool removePoint(EntityId id);
        void forEachLeaf(const std::function<void(std::vector<EntityId>)>&) const;
        void softClear();
        void hardClear();
        ~Node();
    };
    Node root;
    real epsilon;
public:
    explicit Quadtree(AABB worldBounds, size_t threshold = 16, size_t maxDepth = 8, size_t initialCapacity = 64);
    void addShape(EntityId id, const Context& context);
    void addPoint(EntityId id, const Context& context);
    void removeShape(EntityId id);
    void removePoint(EntityId id);
    void forEachLeaf(const std::function<void(std::vector<EntityId>)>&) const;
    void softClear();
    void hardClear();
};

#endif //ADJUSTABLEPHYSICS2D_QUADTREE_H

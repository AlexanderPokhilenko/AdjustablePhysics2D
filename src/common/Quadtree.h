#ifndef ADJUSTABLEPHYSICS2D_QUADTREE_H
#define ADJUSTABLEPHYSICS2D_QUADTREE_H

#include <vector>
#include <functional>
#include "AABB.h"
#include "../base/primitives.h"
#include "../ecs/Context.h"

#ifdef USE_BARNES_HUT
struct MultiBodyWithMass {
    Vector2 centerOfMass;
    real mass;
    MultiBodyWithMass(real mass = 0, Vector2 centerOfMass = {0, 0}) : mass(mass), centerOfMass(centerOfMass) {};
    virtual std::vector<EntityId> const & getIds() const = 0;
    virtual Vector2 getSize() const = 0;
    virtual bool isLeaf() const = 0;
};
#endif

class Quadtree {
private:
    const size_t Threshold;
    const size_t MaxDepth;
    struct Node
#ifdef USE_BARNES_HUT
        : MultiBodyWithMass
#endif
            {
        const AABB bounds;
        Node *children[4];
        std::vector<EntityId> values;
#ifdef USE_CIRCLES_ONLY
        const real radius;
        const Vector2 center;
#endif
#ifdef USE_BARNES_HUT
        inline const std::vector<EntityId> & getIds() const final { return values; };
        inline Vector2 getSize() const final { return bounds.getSize(); };
#endif
    private:
        enum class Direction { NorthEast, NorthWest, SouthWest, SouthEast };
        inline void add(EntityId id) { values.push_back(id); }
#ifdef USE_BARNES_HUT
        inline void updateCenterOfMass() {
            centerOfMass = {0, 0};
            mass = 0;
            for(const auto *child : children) {
                auto m = child->mass;
                centerOfMass += child->centerOfMass * m;
                mass += m;
            }
            centerOfMass /= mass;
        }
        inline void updateCenterOfMass(const Context& context) {
            centerOfMass = {0, 0};
            for(const auto id : values) {
#ifndef USE_EQUAL_MASSES
                auto m = context.getComponent<MassInfoComponent>(id).mass;
                centerOfMass += context.getComponent<LocationComponent>(id).linear * m;
#else
                centerOfMass += context.getComponent<LocationComponent>(id).linear;
#endif
            }
#ifndef USE_EQUAL_MASSES
            centerOfMass /= mass;
#else
            centerOfMass /= values.size();
#endif
        }
        inline void addPointWithMass(EntityId id, const Context& context) {
#ifndef USE_EQUAL_MASSES
            mass += context.getComponent<MassInfoComponent>(id).mass;
#else
            mass += MassInfo::mass;
#endif
            updateCenterOfMass(context);
        }
#endif
    public:
        explicit Node(AABB bounds, size_t capacity = 0);
        inline bool isLeaf() const { return children[0] == nullptr; }
        void createChildren();
        void addShape(EntityId id, const Shape& shape, const Context& context, size_t threshold, size_t maxDepth, size_t depth = 0);
        void addPoint(EntityId id, const Vector2& position, const Context& context, size_t threshold, size_t maxDepth, size_t depth = 0);
        bool removeShape(EntityId id);
        bool removePoint(EntityId id);
        void forEachLeaf(const std::function<void(std::vector<EntityId> const &)>&) const;
        void forEachLeaf(const std::function<void(Node const *)>&) const;
        void forFirstNodesWhere(const std::function<void(Node const *)> &func, const std::function<bool(Node const *)> &predicate) const;
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
#ifdef USE_BARNES_HUT
    inline void forEachLeafWithMass(const std::function<void(MultiBodyWithMass const *)> &func) const { root.forEachLeaf(func); }
    inline void forFirstNodesWithMassWhere(const std::function<void(MultiBodyWithMass const *)> &func, const std::function<bool(MultiBodyWithMass const *)> &predicate) const { root.forFirstNodesWhere(func, predicate); }
#endif
    inline void removeShape(EntityId id) { root.removeShape(id); }
    inline void removePoint(EntityId id) { root.removePoint(id); }
    inline void forEachLeaf(const std::function<void(std::vector<EntityId> const &)> &func) const { root.forEachLeaf(func); }
    inline void softClear() { root.softClear(); }
    inline void hardClear() { root.hardClear(); }
};

#endif //ADJUSTABLEPHYSICS2D_QUADTREE_H

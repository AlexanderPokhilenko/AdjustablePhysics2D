#include "Quadtree.h"
#include <algorithm>

Quadtree::Node::Node(AABB bounds, size_t capacity) : bounds(bounds), children{}
#ifdef USE_CIRCLES_ONLY
, radius(bounds.getHalfSize().getMagnitude()), center(bounds.getCenter())
#endif
{
    values.reserve(capacity);
    for(auto &child : children) child = nullptr;
}

void Quadtree::Node::createChildren() {
#ifndef USE_CIRCLES_ONLY
    auto center = bounds.getCenter();
#endif
    auto childCapacity = values.capacity() / 4;
    children[0] = new Node({center, bounds.max}, childCapacity);
    children[1] = new Node({{bounds.min.x, center.y}, {center.x, bounds.max.y}}, childCapacity);
    children[2] = new Node({bounds.min, center}, childCapacity);
    children[3] = new Node({{center.x, bounds.min.y}, {bounds.max.x, center.y}}, childCapacity);
}

void Quadtree::Node::addShape(EntityId id, const Shape& shape, const Context& context, size_t threshold, size_t maxDepth, size_t depth) {
#ifdef USE_CIRCLES_ONLY
    auto sumR = radius + shape.radius;
    auto displacement = shape.centroid - center;
    if(sumR * sumR < displacement.getSqrMagnitude()) return;
#else
    const auto &aabb = shape.boundingBox;
    if(aabb.max.x < bounds.min.x || bounds.max.x < aabb.min.x ||
       aabb.max.y < bounds.min.y || bounds.max.y < aabb.min.y) return;
#endif
    if(isLeaf()) {
        if(depth >= maxDepth || values.size() < threshold) {
            values.push_back(id);
            return;
        } else {
            createChildren();
            for(auto oldId : values) {
                const auto &oldShape = context.getComponent<ShapeComponent>(oldId);
                for(auto &child : children) child->addShape(oldId, oldShape, context, threshold, maxDepth, depth + 1);
            }
            values.clear();
        }
    }
    for(auto &child : children) child->addShape(id, shape, context, threshold, maxDepth, depth + 1);
}

void Quadtree::Node::addPoint(EntityId id, const Vector2& position, const Context& context, size_t threshold, size_t maxDepth, size_t depth) {
    if(position.x < bounds.min.x || position.x > bounds.max.x ||
       position.y < bounds.min.y || position.y > bounds.max.y) return;
    if(isLeaf()) {
        if(depth >= maxDepth || values.size() < threshold) {
            values.push_back(id);
            return;
        } else {
            createChildren();
            for(auto oldId : values) {
                const auto &oldPosition = context.getComponent<LocationComponent>(oldId).linear;
                for(auto &child : children) child->addPoint(oldId, oldPosition, context, threshold, maxDepth, depth + 1);
            }
            values.clear();
        }
    }
    for(auto &child : children) child->addPoint(id, position, context, threshold, maxDepth, depth + 1);
}

bool Quadtree::Node::removeShape(EntityId id) {
    if(isLeaf()) {
        auto it = std::find(values.begin(), values.end(), id);
        if(it == values.end()) return false;
        values.erase(it);
        return true;
    } else {
        bool removed = false;
        for(auto &child : children) if(child->removeShape(id)) removed = true;
        return removed;
    }
}

bool Quadtree::Node::removePoint(EntityId id) {
    if(isLeaf()) {
        auto it = std::find(values.begin(), values.end(), id);
        if(it == values.end()) return false;
        values.erase(it);
        return true;
    } else {
        for(auto &child : children) if(child->removePoint(id)) return true;
        return false;
    }
}

void Quadtree::Node::forEachLeaf(const std::function<void(std::vector<EntityId>)>&func) const {
    if(isLeaf()) {
        func(values);
    } else {
        for(const auto &child : children) child->forEachLeaf(func);
    }
}

void Quadtree::Node::softClear() {
    values.clear();
    for(auto &child : children) child->softClear();
}

void Quadtree::Node::hardClear() {
    values.clear();
    for (auto &child : children) delete child;
    for(auto &child : children) child = nullptr;
}

Quadtree::Node::~Node() {
    for (auto &child : children) delete child;
}

Quadtree::Quadtree(AABB worldSize, size_t threshold, size_t maxDepth, size_t initialCapacity)
: root(worldSize, initialCapacity), Threshold(threshold), MaxDepth(maxDepth) { }

void Quadtree::addShape(EntityId id, const Context &context) {
    const auto &shape = context.getComponent<ShapeComponent>(id);
#ifdef USE_CIRCLES_ONLY
    auto min = shape.centroid - Vector2{shape.radius, shape.radius}, max = shape.centroid + Vector2{shape.radius, shape.radius};
#else
    auto min = shape.boundingBox.min, max = shape.boundingBox.max;
#endif
    Vector2 displacement{0, 0};

    if(min.x < root.bounds.min.x) {
        displacement.x = root.bounds.min.x - min.x;
    } else if(max.x > root.bounds.max.x) {
        displacement.x = root.bounds.max.x - max.x;
    }

    if(min.y < root.bounds.min.y) {
        displacement.y = root.bounds.min.y - min.y;
    } else if(max.y > root.bounds.max.y) {
        displacement.y = root.bounds.max.y - max.y;
    }

    if(displacement.x == 0 && displacement.y == 0) {
        root.addShape(id, shape, context, Threshold, MaxDepth);
    } else {
        const auto newShape = shape + displacement;
        root.addShape(id, newShape, context, Threshold, MaxDepth);
    }
}

void Quadtree::addPoint(EntityId id, const Context &context) {
    const auto &position = context.getComponent<LocationComponent>(id).linear;
    Vector2 displacement{0, 0};

    if(position.x < root.bounds.min.x) {
        displacement.x = root.bounds.min.x - position.x;
    } else if(position.x > root.bounds.max.x) {
        displacement.x = root.bounds.max.x - position.x;
    }

    if(position.y < root.bounds.min.y) {
        displacement.y = root.bounds.min.y - position.y;
    } else if(position.y > root.bounds.max.y) {
        displacement.y = root.bounds.max.y - position.y;
    }

    if(displacement.x == 0 && displacement.y == 0) {
        root.addPoint(id, position, context, Threshold, MaxDepth);
    } else {
        const auto newPosition = position + displacement;
        root.addPoint(id, newPosition, context, Threshold, MaxDepth);
    }
}

void Quadtree::removeShape(EntityId id) {
    root.removeShape(id);
}

void Quadtree::removePoint(EntityId id) {
    root.removePoint(id);
}

void Quadtree::forEachLeaf(const std::function<void(std::vector<EntityId>)> &func) const {
    root.forEachLeaf(func);
}

void Quadtree::softClear() {
    root.softClear();
}

void Quadtree::hardClear() {
    root.hardClear();
}

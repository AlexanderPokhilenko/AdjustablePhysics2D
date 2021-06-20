#include "Quadtree.h"
#include <algorithm>
#include "../math/MathExtensions.h"

Quadtree::Node::Node(AABB bounds, size_t capacity) : bounds(bounds), children{}
#ifdef USE_CIRCLES_ONLY
, radius(bounds.getHalfSize().getMagnitude()), center(bounds.getCenter())
#endif
#ifdef USE_BARNES_HUT
, MultiBodyWithMass(0, bounds.getCenter())
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
    children[static_cast<size_t>(Direction::NorthEast)] = new Node({center, bounds.max}, childCapacity);
    children[static_cast<size_t>(Direction::NorthWest)] = new Node({{bounds.min.x, center.y}, {center.x, bounds.max.y}}, childCapacity);
    children[static_cast<size_t>(Direction::SouthWest)] = new Node({bounds.min, center}, childCapacity);
    children[static_cast<size_t>(Direction::SouthEast)] = new Node({{center.x, bounds.min.y}, {bounds.max.x, center.y}}, childCapacity);
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
            add(id);
            return;
        } else {
            createChildren();
#ifndef USE_CIRCLES_ONLY
            const auto center = bounds.getCenter();
#endif
            for(auto oldId : values) {
                const auto &oldShape = context.getComponent<ShapeComponent>(oldId);
#ifdef USE_CIRCLES_ONLY
                const auto oldCentroid = oldShape.centroid;
                const auto oldRadius = oldShape.radius;
                const AABB oldAabb = {oldCentroid - Vector2{oldRadius, oldRadius}, oldCentroid + Vector2{oldRadius, oldRadius}};
#else
                const auto oldAabb = oldShape.boundingBox;
#endif
                if(oldAabb.max.x >= center.x && oldAabb.max.y >= center.y) children[static_cast<size_t>(Direction::NorthEast)]->add(oldId);
                if(oldAabb.min.x <= center.x && oldAabb.max.y >= center.y) children[static_cast<size_t>(Direction::NorthWest)]->add(oldId);
                if(oldAabb.min.x <= center.x && oldAabb.min.y <= center.y) children[static_cast<size_t>(Direction::SouthWest)]->add(oldId);
                if(oldAabb.max.x >= center.x && oldAabb.min.y <= center.y) children[static_cast<size_t>(Direction::SouthEast)]->add(oldId);
            }
            values.clear();
        }
    }
    for(auto &child : children) child->addShape(id, shape, context, threshold, maxDepth, depth + 1);
}

void Quadtree::Node::addPoint(EntityId id, const Vector2& position, const Context& context, size_t threshold, size_t maxDepth, size_t depth) {
    if(position.x <= bounds.min.x || position.x > bounds.max.x ||
       position.y <= bounds.min.y || position.y > bounds.max.y) return;
    if(isLeaf()) {
        if(depth >= maxDepth || values.size() < threshold) {
            add(id);
#ifdef USE_BARNES_HUT
            addPointWithMass(id, context);
#endif
            return;
        } else {
            createChildren();
#ifndef USE_CIRCLES_ONLY
            const auto center = bounds.getCenter();
#endif
            for(auto oldId : values) {
                const auto &oldPosition = context.getComponent<LocationComponent>(oldId).linear;
                Node *child = nullptr;
                if(oldPosition.x > center.x) { // East
                    if(oldPosition.y > center.y) { // North
                        child = children[static_cast<size_t>(Direction::NorthEast)];
                    } else { // South
                        child = children[static_cast<size_t>(Direction::SouthEast)];
                    }
                } else { // West
                    if(oldPosition.y > center.y) { // North
                        child = children[static_cast<size_t>(Direction::NorthWest)];
                    } else { // South
                        child = children[static_cast<size_t>(Direction::SouthWest)];
                    }
                }
                child->add(oldId);
#ifdef USE_BARNES_HUT
                child->addPointWithMass(oldId, context);
#endif
            }
            values.clear();
        }
    }
    for(auto &child : children) child->addPoint(id, position, context, threshold, maxDepth, depth + 1);
#ifdef USE_BARNES_HUT
    updateCenterOfMass();
#endif
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

void Quadtree::Node::forEachLeaf(const std::function<void(std::vector<EntityId> const &)>&func) const {
    if(isLeaf()) {
        func(values);
    } else {
        for(const auto &child : children) child->forEachLeaf(func);
    }
}

void Quadtree::Node::forEachLeaf(const std::function<void(Node const *)>&func) const {
    if(isLeaf()) {
        func(this);
    } else {
        for(const auto &child : children) child->forEachLeaf(func);
    }
}

void Quadtree::Node::forFirstNodesWhere(const std::function<void(Node const *)> &func, const std::function<bool(Node const *)> &predicate) const {
    if(predicate(this)) {
        func(this);
    } else if(!isLeaf()) {
        for(const auto &child : children) child->forFirstNodesWhere(func, predicate);
    }
}

void Quadtree::Node::softClear() {
    values.clear();
    if(isLeaf()) return;
    for(auto &child : children) child->softClear();
}

void Quadtree::Node::hardClear() {
    values.clear();
    if(isLeaf()) return;
    for (auto &child : children) delete child;
    for(auto &child : children) child = nullptr;
}

Quadtree::Node::~Node() {
    for (auto &child : children) delete child;
}

Quadtree::Quadtree(AABB worldBounds, size_t threshold, size_t maxDepth, size_t initialCapacity)
: root(worldBounds, initialCapacity), Threshold(threshold), MaxDepth(maxDepth) {
    auto size = worldBounds.getSize();
    auto minSize = size.x < size.y ? size.x : size.y;
    epsilon = minSize / powr(2, real(maxDepth + 2));
    if(epsilon > Epsilon) epsilon = Epsilon;
}

void Quadtree::addShape(EntityId id, const Context &context) {
    const auto &shape = context.getComponent<ShapeComponent>(id);
#ifdef USE_CIRCLES_ONLY
    auto min = shape.centroid, max = shape.centroid;
#else
    auto min = shape.boundingBox.min, max = shape.boundingBox.max;
#endif
    Vector2 displacement{0, 0};

    if(max.x < root.bounds.min.x) {
        displacement.x = root.bounds.min.x - max.x + epsilon;
    } else if(min.x > root.bounds.max.x) {
        displacement.x = root.bounds.max.x - min.x - epsilon;
    }

    if(max.y < root.bounds.min.y) {
        displacement.y = root.bounds.min.y - max.y + epsilon;
    } else if(min.y > root.bounds.max.y) {
        displacement.y = root.bounds.max.y - min.y - epsilon;
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

    if(position.x <= root.bounds.min.x) {
        displacement.x = root.bounds.min.x - position.x + epsilon;
    } else if(position.x > root.bounds.max.x) {
        displacement.x = root.bounds.max.x - position.x - epsilon;
    }

    if(position.y <= root.bounds.min.y) {
        displacement.y = root.bounds.min.y - position.y + epsilon;
    } else if(position.y > root.bounds.max.y) {
        displacement.y = root.bounds.max.y - position.y - epsilon;
    }

    if(displacement.x == 0 && displacement.y == 0) {
        root.addPoint(id, position, context, Threshold, MaxDepth);
    } else {
        const auto newPosition = position + displacement;
        root.addPoint(id, newPosition, context, Threshold, MaxDepth);
    }
}

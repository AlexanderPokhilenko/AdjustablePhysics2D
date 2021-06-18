#include "BroadPhaseSystem.h"

ComponentsBitset BroadPhaseSystem::createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Shape));
    return bitset;
}

#ifdef USE_SPATIAL_HASHING
BroadPhaseSystem::BroadPhaseSystem(size_t length, real sizeX, real sizeY) :
System(createCurrentSystemBitset()), arrayLength(length) {
    if(sizeX <= 0) sizeX = DefaultCellSize;
    if(sizeY <= 0) sizeY = sizeX;
    cellSizeX = sizeX;
    cellSizeY = sizeY;
    inverseCellSizeX = 1 / sizeX;
    inverseCellSizeY = 1 / sizeY;
    cells = new std::unordered_set<EntityId>[arrayLength];
}
#else
BroadPhaseSystem::BroadPhaseSystem() : System(createCurrentSystemBitset()) { }
#endif

void BroadPhaseSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &shape = context.getComponent<ShapeComponent>(id);
#ifdef USE_SPATIAL_HASHING
#ifdef USE_CIRCLES_ONLY
    addCircle(id, shape);
#elif defined(USE_AABB_ONLY)
    addAABB(id, shape);
#else
    if(shape.shapeType == ShapeType::Circle) {
        addCircle(id, shape);
    } else {
        addAABB(id, shape);
    }
#endif
#elif defined(USE_SWEEP_AND_PRUNE)
#ifdef USE_CIRCLES_ONLY
    axisX.insert({shape.centroid.x - shape.radius, id});
    axisX.insert({shape.centroid.x + shape.radius, id});
    axisY.insert({shape.centroid.y - shape.radius, id});
    axisY.insert({shape.centroid.y + shape.radius, id});
#else
    axisX.insert({shape.boundingBox.min.x, id});
    axisX.insert({shape.boundingBox.max.x, id});
    axisY.insert({shape.boundingBox.min.y, id});
    axisY.insert({shape.boundingBox.max.y, id});
#endif
#elif defined(USE_QUADTREE)
#error Not implemented! //TODO
#elif defined(USE_BROAD_PHASE)
#warning Broad phase is disabled!
#endif
}

#ifdef USE_SPATIAL_HASHING
size_t BroadPhaseSystem::getIndex(EntityId id, real x, real y) const {
    long xl, yl;
#ifdef USE_UNIT_CELL_SIZE
    xl = static_cast<long>(x) * P1;
    yl = static_cast<long>(y) * P2;
#else
    xl = static_cast<long>(x * inverseCellSizeX) * P1;
    yl = static_cast<long>(y * inverseCellSizeY) * P2;
#endif
    auto h = (xl ^ yl) % arrayLength;
    return static_cast<size_t>(h);
}

void BroadPhaseSystem::addToCell(EntityId id, real x, real y) {
    cells[getIndex(id, x, y)].insert(id);
}

void BroadPhaseSystem::tryAddToCell(EntityId id, real x, real y) {
    auto& cell = cells[getIndex(id, x, y)];
    cell.insert(id);
    //if(!vector.empty() && vector.back() == id) return;
    //vector.push_back(id);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-flp30-c"
#ifndef USE_CIRCLES_ONLY
void BroadPhaseSystem::addAABB(EntityId id, const Shape& shape) {
    auto boundingBox = shape.boundingBox;
    for (real x = boundingBox.min.x - cellSizeX; x <= boundingBox.max.x + cellSizeX; x += cellSizeX) {
        for (real y = boundingBox.min.y - cellSizeY; y <= boundingBox.max.y + cellSizeY; y += cellSizeY) {
            addToCell(id, x, y);
        }
    }
}
#endif
#ifndef USE_AABB_ONLY
void BroadPhaseSystem::addCircle(EntityId id, const Shape& shape) {
    //Midpoint circle algorithm
    real step = cellSizeY > cellSizeX ? cellSizeX : cellSizeY;
    real x0 = shape.centroid.x;
    real y0 = shape.centroid.y;
    real x = shape.radius + step;
    real y = 0;
    real err = 0;
    float i = 0;

    while (x >= y) {
        for(real dx = x0 + i * cellSizeX; dx <= x0 + x; dx += cellSizeX) {
            tryAddToCell(id, dx, y0 + y);
            tryAddToCell(id, dx, y0 - y);
        }
        for(real dy = y0 + i * cellSizeY; dy <= y0 + x; dy += cellSizeY) {
            tryAddToCell(id, x0 + y, dy);
            tryAddToCell(id, x0 - y, dy);
        }
        for(real dx = x0 - i * cellSizeX; dx >= x0 - x; dx -= cellSizeX) {
            tryAddToCell(id, dx, y0 + y);
            tryAddToCell(id, dx, y0 - y);
        }
        for(real dy = y0 - i * cellSizeY; dy >= y0 - x; dy -= cellSizeY) {
            tryAddToCell(id, x0 - y, dy);
            tryAddToCell(id, x0 + y, dy);
        }
        if (err <= 0) {
            y += step;
            err += 2 * y + step;
        }
        if (err > 0) {
            x -= step;
            err -= 2 * x + step;
        }
        i++;
    }
}
#endif
#pragma clang diagnostic pop

void BroadPhaseSystem::setArrayLength(size_t length) {
    delete[] cells;
    arrayLength = length;
    cells = new std::unordered_set<EntityId>[length];
}

#ifndef USE_UNIT_CELL_SIZE

void BroadPhaseSystem::setCellSize(real size) {
    setCellSize(size, size);
}

void BroadPhaseSystem::setCellSize(real sizeX, real sizeY) {
    if(sizeX <= 0) sizeX = DefaultCellSize;
    if(sizeY <= 0) sizeY = sizeX;
    cellSizeX = sizeX;
    cellSizeY = sizeY;
    inverseCellSizeX = 1 / sizeX;
    inverseCellSizeY = 1 / sizeY;
}
#else
BroadPhaseSystem::BroadPhaseSystem(size_t length) :
System(createCurrentSystemBitset()), arrayLength(length) {
    cells = new std::vector<EntityId>[arrayLength];
}
#endif
#elif defined(USE_SWEEP_AND_PRUNE)
void BroadPhaseSystem::fillPairsFromAxis(const std::multimap<real, EntityId>& axis, unordered_pairs_set& set) {
    std::unordered_set<EntityId> active;
    for (const auto &item : axis) {
        const auto id = item.second;
        const auto cit = active.find(id);
        if(cit == active.end()) {
            for (const auto activeId : active) {
                if(activeId > id) {
                    set.insert(std::make_pair(id, activeId));
                } else {
                    set.insert(std::make_pair(activeId, id));
                }
            }
            active.insert(id);
        } else {
            active.erase(cit);
        }
    }
}
#endif

void BroadPhaseSystem::update(Context &context, real deltaTime) {
#ifdef USE_BROAD_PHASE
    context.possibleCollisions.clear();
#endif
#ifdef USE_SPATIAL_HASHING
    for (size_t i = 0; i < arrayLength; ++i) cells[i].clear();
#elif defined(USE_SWEEP_AND_PRUNE)
    axisX.clear();
    axisY.clear();
#endif
    System::update(context, deltaTime);
#ifdef USE_SPATIAL_HASHING
    unordered_pairs_set set;
    for (size_t i = 0; i < arrayLength; ++i) {
        auto &currentCell = cells[i];
        auto size = cells[i].size();
        if(size < 2) continue;
        for (auto it1 = currentCell.begin(); it1 != currentCell.end(); ++it1) {
            auto it2 = it1;
            for (++it2; it2 != currentCell.end(); ++it2) {
                auto e1 = *it1, e2 = *it2;
#ifdef USE_COLLISION_FILTER
                if(context.hasComponent<CollisionFilterComponent>(e1) && context.hasComponent<CollisionFilterComponent>(e2)) {
                    auto &f1 = context.getComponent<CollisionFilterComponent>(e1);
                    auto &f2 = context.getComponent<CollisionFilterComponent>(e2);
                    if(!f1.check(f2)) continue;
                }
#endif
                if(e1 < e2) {
                    set.insert(std::make_pair(e1, e2));
                } else {
                    set.insert(std::make_pair(e2, e1));
                }
            }
        }
    }
    context.possibleCollisions.insert(context.possibleCollisions.end(), set.begin(), set.end());
#elif defined(USE_SWEEP_AND_PRUNE)
    unordered_pairs_set pairsX, pairsY;

    fillPairsFromAxis(axisX, pairsX);
    fillPairsFromAxis(axisY, pairsY);

    for (const auto &pair : pairsX) {
        if(pairsY.find(pair) != pairsY.end()) {
#ifdef USE_COLLISION_FILTER
            auto e1 = pair.first, e2 = pair.second;
            if(context.hasComponent<CollisionFilterComponent>(e1) && context.hasComponent<CollisionFilterComponent>(e2)) {
                    auto &f1 = context.getComponent<CollisionFilterComponent>(e1);
                    auto &f2 = context.getComponent<CollisionFilterComponent>(e2);
                    if(!f1.check(f2)) continue;
                }
#endif
            context.possibleCollisions.push_back(pair);
        }
    }
#endif
}

BroadPhaseSystem::~BroadPhaseSystem() {
#ifdef USE_SPATIAL_HASHING
    delete[] cells;
#endif
}

#include "BroadPhaseSystem.h"

ComponentsBitset createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Shape));
    return bitset;
}

#ifdef USE_SPATIAL_HASHING
#include <unordered_set>
struct pair_hash {
    inline std::size_t operator()(const std::pair<EntityId ,EntityId> & v) const {
        return v.first * 31 + v.second;
    }
};

BroadPhaseSystem::BroadPhaseSystem(size_t length, real sizeX, real sizeY) :
System(createCurrentSystemBitset()), arrayLength(length) {
    if(sizeX <= 0) sizeX = DefaultCellSize;
    if(sizeY <= 0) sizeY = sizeX;
    cellSizeX = sizeX;
    cellSizeY = sizeY;
    inverseCellSizeX = 1 / sizeX;
    inverseCellSizeY = 1 / sizeY;
    cells = new std::vector<EntityId>[arrayLength];
}
#else
BroadPhaseSystem::BroadPhaseSystem() : System(createCurrentSystemBitset()) { }
#endif

void BroadPhaseSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &shape = context.getComponent<Shape>(id);
#ifdef USE_QUADTREE
#error Not implemented! //TODO
#elif defined(USE_SPATIAL_HASHING)

#ifdef USE_CIRCLES_ONLY
    addCircle(id, shape);
#elif defined(USE_AABB_ONLY)
    addAABB(id, shape);
#else
    if(shape.shapeType == ShapeType::Circle){
        addCircle(id, shape);
    } else {
        addAABB(id, shape);
    }
#endif

#elif defined(USE_SWEEP_AND_PRUNE)
#error Not implemented! //TODO
#else
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
    cells[getIndex(id, x, y)].push_back(id);
}

void BroadPhaseSystem::tryAddToCell(EntityId id, real x, real y) {
    auto& vector = cells[getIndex(id, x, y)];
    if(!vector.empty() && vector.back() == id) return;
    vector.push_back(id);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-flp30-c"
#ifndef USE_CIRCLES_ONLY
void BroadPhaseSystem::addAABB(EntityId id, const Shape& shape) {
    auto boundingBox = shape.boundingBox;
    for (real x = boundingBox.min.x; x < boundingBox.max.x; x += cellSizeX) {
        for (real y = boundingBox.min.y; y < boundingBox.max.y; y += cellSizeY) {
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
    real x = shape.radius;
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
    cells = new std::vector<EntityId>[length];
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
#endif

void BroadPhaseSystem::update(Context &context, real deltaTime) {
#ifdef USE_SPATIAL_HASHING
    for (size_t i = 0; i < arrayLength; ++i) cells[i].clear();
#endif
    System::update(context, deltaTime);
#ifdef USE_SPATIAL_HASHING
    context.possibleCollisions.clear();
    std::unordered_set<std::pair<EntityId, EntityId>, pair_hash> set;
    for (size_t i = 0; i < arrayLength; ++i) {
        auto size = cells[i].size();
        if(size < 2) continue;
        for (int e1 = 0; e1 < size; ++e1) {
            for (int e2 = e1 + 1; e2 < size; ++e2) {
                if(e1 < e2) {
                    set.insert(std::make_pair(e1, e2));
                } else {
                    set.insert(std::make_pair(e2, e1));
                }
            }
        }
    }
    context.possibleCollisions.insert(context.possibleCollisions.end(), set.begin(), set.end());
#endif
}

BroadPhaseSystem::~BroadPhaseSystem() {
#ifdef USE_SPATIAL_HASHING
    delete[] cells;
#endif
}

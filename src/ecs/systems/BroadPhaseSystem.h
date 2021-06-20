#ifndef ADJUSTABLEPHYSICS2D_BROADPHASESYSTEM_H
#define ADJUSTABLEPHYSICS2D_BROADPHASESYSTEM_H

#include "System.h"
#ifdef USE_SWEEP_AND_PRUNE
#include <map>
#elif defined(USE_QUADTREE)
#include "../../common/Quadtree.h"
#endif
#include <unordered_set>

class BroadPhaseSystem : public System {
private:
#ifdef USE_QUADTREE
    Quadtree *const quadtree;
    const size_t hardClearPeriod;
    size_t iterationNumber;
#endif
    static ComponentsBitset createCurrentSystemBitset();
    struct pair_hash {
        inline std::size_t operator()(const std::pair<EntityId ,EntityId> & p) const {
            return p.first * 31 + p.second;
        }
    };
    using unordered_pairs_set = std::unordered_set<std::pair<EntityId, EntityId>, pair_hash>;
#ifdef USE_SWEEP_AND_PRUNE
    std::multimap<real, EntityId> axisX;
    std::multimap<real, EntityId> axisY;
    static void fillPairsFromAxis(const std::multimap<real, EntityId>& axis, unordered_pairs_set& set);
#elif defined(USE_SPATIAL_HASHING)
    static const long P1 = 73856093;
    static const long P2 = 19349663;
#ifndef USE_UNIT_CELL_SIZE
    constexpr static const real DefaultCellSize = 2;
    real cellSizeX;
    real cellSizeY;
    real inverseCellSizeX;
    real inverseCellSizeY;
#else
    constexpr static const real cellSizeX = 1;
    constexpr static const real cellSizeY = 1;
#endif
    size_t arrayLength;
    std::unordered_set<EntityId>* cells;
    size_t getIndex(EntityId id, real x, real y) const;
    void addToCell(EntityId id, real x, real y);
    void tryAddToCell(EntityId id, real x, real y);
#ifndef USE_CIRCLES_ONLY
    void addAABB(EntityId id, const Shape& shape);
#endif
#ifndef USE_AABB_ONLY
    void addCircle(EntityId id, const Shape& shape);
#endif
#endif
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
#ifdef USE_SPATIAL_HASHING
#ifndef USE_UNIT_CELL_SIZE
    explicit BroadPhaseSystem(size_t arrayLength = 32, real cellSizeX = DefaultCellSize, real cellSizeY = 0);
    void setCellSize(real size);
    void setCellSize(real x, real y);
#else
    explicit BroadPhaseSystem(size_t arrayLength = 32);
#endif
    void setArrayLength(size_t length);
#elif defined(USE_SWEEP_AND_PRUNE)
    BroadPhaseSystem();
#elif defined(USE_QUADTREE)
    BroadPhaseSystem(Quadtree *quadtree = nullptr, size_t hardClearPeriod = 15);
#endif
    void update(Context &context, real deltaTime) override;
    ~BroadPhaseSystem() override;
};

#endif //ADJUSTABLEPHYSICS2D_BROADPHASESYSTEM_H

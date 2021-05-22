#ifndef ADJUSTABLEPHYSICS2D_BROADPHASESYSTEM_H
#define ADJUSTABLEPHYSICS2D_BROADPHASESYSTEM_H

#include "System.h"

class BroadPhaseSystem : public System {
private:
#ifdef USE_SPATIAL_HASHING
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
    std::vector<EntityId>* cells;
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
#ifndef USE_SPATIAL_HASHING
    BroadPhaseSystem();
#else
#ifndef USE_UNIT_CELL_SIZE
    explicit BroadPhaseSystem(size_t arrayLength = 32, real cellSizeX = DefaultCellSize, real cellSizeY = 0);
    void setCellSize(real size);
    void setCellSize(real x, real y);
#else
    explicit BroadPhaseSystem(size_t arrayLength = 32);
#endif
    void setArrayLength(size_t length);
#endif
    void update(Context &context, real deltaTime) override;
    ~BroadPhaseSystem() override;
};

#endif //ADJUSTABLEPHYSICS2D_BROADPHASESYSTEM_H

#ifndef ADJUSTABLEPHYSICS2D_GRAVITATIONALFORCESYSTEM_H
#define ADJUSTABLEPHYSICS2D_GRAVITATIONALFORCESYSTEM_H

#include "System.h"
#ifdef USE_BARNES_HUT
#include "../../common/Quadtree.h"
#endif

class GravitationalForceSystem : public System {
private:
    static ComponentsBitset createCurrentSystemBitset();
#ifdef USE_BARNES_HUT
    Quadtree *const quadtree;
#ifndef USE_QUADTREE_FOR_BROADPHASE
    const size_t hardClearPeriod;
    size_t iterationNumber;
#endif
#endif
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
#ifdef USE_BARNES_HUT
    void update(Context &context, real deltaTime) override;
#ifndef USE_QUADTREE_FOR_BROADPHASE
    GravitationalForceSystem(Quadtree *quadtree = nullptr, size_t hardClearPeriod = 15);
#else
    GravitationalForceSystem(Quadtree *quadtree = nullptr);
#endif
#else
    GravitationalForceSystem();
#endif
    real G;
#ifdef USE_BARNES_HUT
    real theta;
#endif
};

#endif //ADJUSTABLEPHYSICS2D_GRAVITATIONALFORCESYSTEM_H

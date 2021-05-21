#ifndef ADJUSTABLEPHYSICS2D_PHYSICSENGINE_H
#define ADJUSTABLEPHYSICS2D_PHYSICSENGINE_H

#include "ecs/Context.h"
#include "ecs/systems/System.h"
#include "ecs/systems/SystemType.h"

class PhysicsEngine {
private:
    Context context;
    System* systems[SystemsCount];
public:
    explicit PhysicsEngine(size_t entitiesCapacity = 64, size_t freeCapacity = 16);
    void simulate(real deltaTime);
    ~PhysicsEngine();
};

#endif //ADJUSTABLEPHYSICS2D_PHYSICSENGINE_H

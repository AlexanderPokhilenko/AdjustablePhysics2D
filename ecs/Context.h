#ifndef ADJUSTABLEPHYSICS2D_CONTEXT_H
#define ADJUSTABLEPHYSICS2D_CONTEXT_H

#include <vector>
#include <bitset>
#include "../base/primitives.h"
#include "components/Components.h"
#include "components/ComponentType.h"

class Context {
friend class System;
private:
    const std::vector<EntityId> freeIds;
    const std::vector<ComponentsBitset> inUse;
    const std::vector<ShapeComponent> shapes;
    const std::vector<MassInfoComponent> masses;
    const std::vector<MaterialComponent> materials;
    const std::vector<PolygonComponent> polygons;
    const std::vector<LocationComponent> locations;
    const std::vector<VelocityComponent> velocities;
    const std::vector<AccelerationComponent> accelerations;
    const std::vector<ForceComponent> forces;
public:
    explicit Context(size_t entitiesCapacity = 64, size_t freeCapacity = 16);
};

#endif //ADJUSTABLEPHYSICS2D_CONTEXT_H

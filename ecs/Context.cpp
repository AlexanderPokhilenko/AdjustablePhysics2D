#include "Context.h"

Context::Context(size_t entitiesCapacity, size_t freeCapacity) : freeIds(freeCapacity), inUse(entitiesCapacity),
    shapes(entitiesCapacity), masses(entitiesCapacity), materials(entitiesCapacity), polygons(entitiesCapacity),
    locations(entitiesCapacity), velocities(entitiesCapacity), accelerations(entitiesCapacity), forces(entitiesCapacity)
    {}
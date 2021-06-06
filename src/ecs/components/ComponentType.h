#ifndef ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H
#define ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H

#include <bitset>

enum class ComponentType {
    Shape,
    MassInfo,
#ifdef USE_MATERIAL
    Material,
#endif
    Polygon,
    Location,
    Velocity,
#ifdef USE_ACCELERATION
    Acceleration,
#endif
#ifdef USE_COLLISION_FILTER
    CollisionFilter,
#endif
#ifdef USE_CONSTRAINT
    Constraint,
#endif
    COUNT
};

const std::size_t ComponentsCount = static_cast<std::size_t>(ComponentType::COUNT);
using ComponentsBitset = std::bitset<ComponentsCount>;

#endif //ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H

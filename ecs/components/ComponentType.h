#ifndef ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H
#define ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H

#include <bitset>

enum class ComponentType {
    Shape,
    MassInfo,
    Material,
    Polygon,
    Location,
    Velocity,
    Acceleration,
    COUNT
};

const std::size_t ComponentsCount = static_cast<std::size_t>(ComponentType::COUNT);
using ComponentsBitset = std::bitset<ComponentsCount>;

#endif //ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H

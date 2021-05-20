#ifndef ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H
#define ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H

enum class ComponentType {
    Shape,
    MassInfo,
    Material,
    Polygon,
    Location,
    Velocity,
    Acceleration,
    Force,
    COUNT
};

using ComponentsBitset = std::bitset<static_cast<std::size_t>(ComponentType::COUNT)>;

#endif //ADJUSTABLEPHYSICS2D_COMPONENTTYPE_H

#ifndef ADJUSTABLEPHYSICS2D_COMPONENT_H
#define ADJUSTABLEPHYSICS2D_COMPONENT_H

#include "ComponentType.h"

template<typename T, ComponentType Type>
struct Component {
    static constexpr auto type = static_cast<std::size_t>(Type);
};

#endif //ADJUSTABLEPHYSICS2D_COMPONENT_H

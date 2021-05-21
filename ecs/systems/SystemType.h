#ifndef ADJUSTABLEPHYSICS2D_SYSTEMTYPE_H
#define ADJUSTABLEPHYSICS2D_SYSTEMTYPE_H

enum class SystemType {
    Move,
#ifdef USE_GLOBAL_GRAVITATION
    GlobalGravitation,
#endif
    COUNT
};

const std::size_t SystemsCount = static_cast<std::size_t>(SystemType::COUNT);

#endif //ADJUSTABLEPHYSICS2D_SYSTEMTYPE_H

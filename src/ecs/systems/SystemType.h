#ifndef ADJUSTABLEPHYSICS2D_SYSTEMTYPE_H
#define ADJUSTABLEPHYSICS2D_SYSTEMTYPE_H

enum class SystemType {
    Move,
#ifdef USE_GLOBAL_GRAVITATION
    GlobalGravitation,
#endif
#ifdef USE_GRAVITATIONAL_FORCE
    GravitationalForce,
#endif
    ShapeTranslation,
#ifdef USE_BROAD_PHASE
    BroadPhase,
#endif
    NarrowPhase,
    Collision,
#ifdef USE_JOINT
    Joint,
#endif
#ifdef USE_CONSTRAINT
    Constraint,
#endif
    COUNT
};

const std::size_t SystemsCount = static_cast<std::size_t>(SystemType::COUNT);

#endif //ADJUSTABLEPHYSICS2D_SYSTEMTYPE_H

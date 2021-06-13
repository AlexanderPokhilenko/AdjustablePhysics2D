#ifndef ADJUSTABLEPHYSICS2D_SHAPETYPE_H
#define ADJUSTABLEPHYSICS2D_SHAPETYPE_H

enum class ShapeType {
    Circle,
    AABB,
#ifndef USE_PRIMITIVES_ONLY
    Complex,
#endif
    COUNT
};

const std::size_t ShapesCount =
#if !defined(USE_CIRCLES_ONLY) && !defined(USE_AABB_ONLY)
    static_cast<std::size_t>(ShapeType::COUNT);
#else
    1;
#endif

#endif //ADJUSTABLEPHYSICS2D_SHAPETYPE_H

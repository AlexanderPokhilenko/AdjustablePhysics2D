#include "ShapeTranslationSystem.h"

ComponentsBitset ShapeTranslationSystem::createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Location));
    bitset.set(static_cast<size_t>(ComponentType::Shape));
    return bitset;
}

ShapeTranslationSystem::ShapeTranslationSystem() : System(createCurrentSystemBitset())
{ }

void ShapeTranslationSystem::update(Context &context, EntityId id, real deltaTime) {
    auto &location = context.getComponent<LocationComponent>(id);
    auto &shape = context.getComponent<ShapeComponent>(id);

#if !defined(USE_PRIMITIVES_ONLY) && !defined(USE_CIRCLES_ONLY) && defined(USE_ROTATION)
    if(shape.shapeType == ShapeType::Complex) {
        auto &polygon = context.getComponent<PolygonComponent>(id);
        shape.boundingBox = polygon.getAABB(location.angular) + location.linear;
#ifndef USE_AABB_ONLY
        shape.centroid = location.linear;
#endif
    } else
#endif
    {
#ifndef USE_CIRCLES_ONLY
        auto displacement = location.linear - shape.getCenter();
        shape += displacement;
#else
        shape.centroid = location.linear;
#endif
    }
}

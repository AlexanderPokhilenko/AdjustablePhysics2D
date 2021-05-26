#include "Context.h"

Context::Context(size_t entitiesCapacity, size_t freeCapacity) : freeIds(), inUse(), components()
{
    freeIds.reserve(freeCapacity);
    inUse.reserve(entitiesCapacity);
#if USE_BROAD_PHASE
    possibleCollisions.reserve(entitiesCapacity);
#endif
    collisions.reserve(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Shape)] = new ComponentContainer<ShapeComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::MassInfo)] = new ComponentContainer<MassInfoComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Material)] = new ComponentContainer<MaterialComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Polygon)] = new ComponentContainer<PolygonComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Location)] = new ComponentContainer<LocationComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Velocity)] = new ComponentContainer<VelocityComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Acceleration)] = new ComponentContainer<AccelerationComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Force)] = new ComponentContainer<ForceComponent>(entitiesCapacity);
}

size_t Context::getEntitiesSize() {
    return inUse.size();
}

bool Context::checkEntity(EntityId id, ComponentsBitset bitset) {
    return (inUse[id] & bitset) == bitset;
}

EntityId Context::createEntity() {
    EntityId id;
    if(freeIds.empty()) {
        id = static_cast<EntityId>(inUse.size());
        inUse.emplace_back();
    } else {
        id = freeIds.back();
        freeIds.pop_back();
        inUse[id].reset();
    }
    return id;
}

void Context::deleteEntity(EntityId id) {
    inUse[id].reset();
    freeIds.push_back(id);
}

Context::~Context() {
    for(auto & component : components) {
        delete component;
    }
}
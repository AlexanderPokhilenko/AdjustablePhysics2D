#include "Context.h"

Context::Context(size_t entitiesCapacity, size_t freeCapacity) : freeIds(), inUse(), components()
#ifdef USE_BROAD_PHASE
    , possibleCollisions()
#endif
#ifdef USE_JOINT
    , joints(freeCapacity)
#endif
{
    freeIds.reserve(freeCapacity);
    inUse.reserve(entitiesCapacity);
#ifdef USE_BROAD_PHASE
    possibleCollisions.reserve(entitiesCapacity);
#endif
    collisions.reserve(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Shape)] = new ComponentContainer<ShapeComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::MassInfo)] = new ComponentContainer<MassInfoComponent>(entitiesCapacity);
#ifdef USE_MATERIAL
    components[static_cast<std::size_t>(ComponentType::Material)] = new ComponentContainer<MaterialComponent>(entitiesCapacity);
#endif
    components[static_cast<std::size_t>(ComponentType::Polygon)] = new ComponentContainer<PolygonComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Location)] = new ComponentContainer<LocationComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Velocity)] = new ComponentContainer<VelocityComponent>(entitiesCapacity);
#ifdef USE_ACCELERATION
    components[static_cast<std::size_t>(ComponentType::Acceleration)] = new ComponentContainer<AccelerationComponent>(entitiesCapacity);
#endif
#ifdef USE_COLLISION_FILTER
    components[static_cast<std::size_t>(ComponentType::CollisionFilter)] = new ComponentContainer<CollisionFilterComponent>(entitiesCapacity);
#endif
#ifdef USE_CONSTRAINT
    components[static_cast<std::size_t>(ComponentType::Constraint)] = new ComponentContainer<ConstraintComponent>(entitiesCapacity);
#endif
}

size_t Context::getEntitiesSize() {
    return inUse.size();
}

bool Context::checkEntity(EntityId id, ComponentsBitset bitset) {
    return (inUse[id] & bitset) == bitset;
}

bool Context::hasEntity(EntityId id) const {
    if(id >= inUse.size()) return false;
    for (auto freeId : freeIds) {
        if(freeId == id) return false;
    }
    return true;
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

bool Context::hasComponent(EntityId id, ComponentType type) const {
    return inUse[id].test(static_cast<std::size_t>(type));
}

void Context::removeComponent(EntityId id, ComponentType type) {
    inUse[id].reset(static_cast<std::size_t>(type));
}

Context::~Context() {
    for(auto & component : components) {
        delete component;
    }
}
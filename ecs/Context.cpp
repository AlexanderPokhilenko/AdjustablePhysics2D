#include "Context.h"

Context::Context(size_t entitiesCapacity, size_t freeCapacity) : freeIds(freeCapacity), inUse(entitiesCapacity), components()
{
    components[static_cast<std::size_t>(ComponentType::Shape)] = new ComponentContainer<ShapeComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::MassInfo)] = new ComponentContainer<MassInfoComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Material)] = new ComponentContainer<MaterialComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Polygon)] = new ComponentContainer<PolygonComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Location)] = new ComponentContainer<LocationComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Velocity)] = new ComponentContainer<VelocityComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Acceleration)] = new ComponentContainer<AccelerationComponent>(entitiesCapacity);
    components[static_cast<std::size_t>(ComponentType::Force)] = new ComponentContainer<ForceComponent>(entitiesCapacity);
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
    freeIds.push_back(id);
}

template<typename T>
ComponentContainer<T> Context::getComponents() {
    return static_cast<ComponentContainer<T>>(components[T::type]);
}

template<typename T, typename... Args>
void Context::addComponent(EntityId id, Args &&... args) {
    getComponents<T>().add(id, args...);
    inUse[id].set(T::type, true);
}

template<typename T>
void Context::removeComponent(EntityId id) {
    inUse[id].reset(T::type);
}

Context::~Context() {
    for(auto & component : components) {
        delete component;
    }
}
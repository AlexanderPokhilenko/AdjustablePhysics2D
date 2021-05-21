#include "ComponentContainer.h"

template<typename T>
ComponentContainer<T>::ComponentContainer(std::size_t capacity) : components(capacity) { }

template<typename T>
T &ComponentContainer<T>::get(EntityId id) {
    return components[id];
}

template<typename T>
const T &ComponentContainer<T>::get(EntityId id) const {
    return components[id];
}

template<typename T>
template<typename... Args>
void ComponentContainer<T>::add(EntityId entity, Args &&... args) {
    components.template emplace_back(std::forward<Args>(args)...);
    components[entity] = std::move(components.back());
    components.pop_back();
}

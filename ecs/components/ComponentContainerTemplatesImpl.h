#ifndef PHYSICSGUI1_COMPONENTCONTAINERTEMPLATESIMPL_H
#define PHYSICSGUI1_COMPONENTCONTAINERTEMPLATESIMPL_H

template<typename T>
ComponentContainer<T>::ComponentContainer(std::size_t capacity) : components() {
    components.reserve(capacity);
}

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
void ComponentContainer<T>::add(EntityId id, Args &&... args) {
    auto index = components.size();
    if(id <= index) {
        components.template emplace_back(std::forward<Args>(args)...);
        if(id < index) {
            components[id] = std::move(components.back());
            components.pop_back();
        }
    } else {
        components.resize((size_t)id);
        components.template emplace(components.end(), std::forward<Args>(args)...);
    }
}

#endif //PHYSICSGUI1_COMPONENTCONTAINERTEMPLATESIMPL_H

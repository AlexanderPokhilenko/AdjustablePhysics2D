#ifndef ADJUSTABLEPHYSICS2D_COMPONENTCONTAINERTEMPLATESIMPL_H
#define ADJUSTABLEPHYSICS2D_COMPONENTCONTAINERTEMPLATESIMPL_H

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
    if(id >= components.size()) components.resize((size_t)(id + 1));
    components[id] = T(args...);
}

#endif //ADJUSTABLEPHYSICS2D_COMPONENTCONTAINERTEMPLATESIMPL_H

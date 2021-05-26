#ifndef PHYSICSGUI1_CONTEXTTEMPLATESIMPL_H
#define PHYSICSGUI1_CONTEXTTEMPLATESIMPL_H

template<typename T>
ComponentContainer<T> *Context::getComponents() {
    return static_cast<ComponentContainer<T>*>(components[T::type]);
}

template<typename T>
const ComponentContainer<T> *Context::getComponents() const {
    return static_cast<ComponentContainer<T>*>(components[T::type]);
}

template<typename T>
bool Context::hasComponent(EntityId id) const {
    return inUse[id].test(T::type);
}

template<typename T>
T &Context::getComponent(EntityId id) {
    return getComponents<T>()->get(id);
}

template<typename T>
const T &Context::getComponent(EntityId id) const {
    return getComponents<T>()->get(id);
}

template<typename T, typename... Args>
void Context::addComponent(EntityId id, Args &&... args) {
    getComponents<T>()->add(id, args...);
    inUse[id].set(T::type);
}

template<typename T>
void Context::removeComponent(EntityId id) {
    inUse[id].reset(T::type);
}

#endif //PHYSICSGUI1_CONTEXTTEMPLATESIMPL_H

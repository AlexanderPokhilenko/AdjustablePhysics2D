#ifndef ADJUSTABLEPHYSICS2D_COMPONENTCONTAINER_H
#define ADJUSTABLEPHYSICS2D_COMPONENTCONTAINER_H

#include <cstddef>
#include <vector>
#include "BaseComponentContainer.h"
#include "../../base/primitives.h"

template<typename T>
class ComponentContainer : public BaseComponentContainer {
private:
    std::vector<T> components;
public:
    explicit ComponentContainer(std::size_t capacity = 64);
    T& get(EntityId id);
    const T& get(EntityId id) const;
    template<typename... Args>
    void add(EntityId id, Args&&... args);
};

#endif //ADJUSTABLEPHYSICS2D_COMPONENTCONTAINER_H

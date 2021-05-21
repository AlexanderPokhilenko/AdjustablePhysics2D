#ifndef ADJUSTABLEPHYSICS2D_CONTEXT_H
#define ADJUSTABLEPHYSICS2D_CONTEXT_H

#include <vector>
#include <bitset>
#include "../base/primitives.h"
#include "components/Components.h"
#include "components/ComponentType.h"
#include "components/ComponentContainer.h"

class Context {
friend class System;
private:
    std::vector<EntityId> freeIds;
    std::vector<ComponentsBitset> inUse;
    BaseComponentContainer* components[ComponentsCount];
    template<typename T>
    ComponentContainer<T>* getComponents();
    template<typename T>
    const ComponentContainer<T>* getComponents() const;
public:
    explicit Context(size_t entitiesCapacity = 64, size_t freeCapacity = 16);
    EntityId createEntity();
    void deleteEntity(EntityId id);
    template<typename T>
    T& getComponent(EntityId id);
    template<typename T>
    const T& getComponent(EntityId id) const;
    template<typename T, typename... Args>
    void addComponent(EntityId id, Args&&... args);
    template<typename T>
    void removeComponent(EntityId id);
    ~Context();
};

#endif //ADJUSTABLEPHYSICS2D_CONTEXT_H

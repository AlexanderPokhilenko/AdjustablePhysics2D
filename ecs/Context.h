#ifndef ADJUSTABLEPHYSICS2D_CONTEXT_H
#define ADJUSTABLEPHYSICS2D_CONTEXT_H

#include <vector>
#include <bitset>
#include "../base/primitives.h"
#include "components/Components.h"
#include "components/ComponentType.h"
#include "components/ComponentContainer.h"
#ifdef USE_JOINT
#include "../common/Joint.h"
#include "components/DoubleKeyContainer.h"
#endif
#include "../common/Collision.h"

class Context {
friend class System;
#ifdef USE_BROAD_PHASE
friend class BroadPhaseSystem;
#endif
friend class NarrowPhaseSystem;
friend class CollisionSystem;
#ifdef USE_JOINT
friend class JointSystem;
#endif
private:
    std::vector<EntityId> freeIds;
    std::vector<ComponentsBitset> inUse;
    BaseComponentContainer* components[ComponentsCount];
#ifdef USE_BROAD_PHASE
    std::vector<std::pair<EntityId, EntityId>> possibleCollisions;
#endif
    std::vector<Collision> collisions;
    template<typename T>
    ComponentContainer<T>* getComponents();
    template<typename T>
    const ComponentContainer<T>* getComponents() const;
public:
#ifdef USE_JOINT
    DoubleKeyContainer<EntityId, Joint> joints;
#endif
    explicit Context(size_t entitiesCapacity = 64, size_t freeCapacity = 16);
    size_t getEntitiesSize();
    bool checkEntity(EntityId id, ComponentsBitset bitset);
    bool hasEntity(EntityId id) const;
    EntityId createEntity();
    void deleteEntity(EntityId id);
    template<typename T>
    bool hasComponent(EntityId id) const;
    bool hasComponent(EntityId id, ComponentType type) const;
    template<typename T>
    T& getComponent(EntityId id);
    template<typename T>
    const T& getComponent(EntityId id) const;
    template<typename T, typename... Args>
    void addComponent(EntityId id, Args&&... args);
    template<typename T>
    void removeComponent(EntityId id);
    void removeComponent(EntityId id, ComponentType type);
    ~Context();
};
#include "ContextTemplatesImpl.h"
#endif //ADJUSTABLEPHYSICS2D_CONTEXT_H

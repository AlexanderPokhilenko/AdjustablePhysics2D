#include "GravitationalForceSystem.h"

ComponentsBitset GravitationalForceSystem::createCurrentSystemBitset()
{
    ComponentsBitset bitset;
    bitset.set(static_cast<size_t>(ComponentType::Location));
    bitset.set(static_cast<size_t>(ComponentType::Velocity));
    bitset.set(static_cast<size_t>(ComponentType::MassInfo));
    return bitset;
}

#ifdef USE_BARNES_HUT
#ifndef USE_QUADTREE_FOR_BROADPHASE
GravitationalForceSystem::GravitationalForceSystem(Quadtree *quadtree, size_t hardClearPeriod) :
#else
GravitationalForceSystem::GravitationalForceSystem(Quadtree *quadtree) :
#endif
System(createCurrentSystemBitset()), G(6.6743e-11), theta(1),
quadtree(quadtree != nullptr ? quadtree : new Quadtree({{-100, -100}, {100, 100}}, 1, 10))
#ifndef USE_QUADTREE_FOR_BROADPHASE
, hardClearPeriod(hardClearPeriod), iterationNumber(0)
#endif
{ }
#else
GravitationalForceSystem::GravitationalForceSystem() : System(createCurrentSystemBitset()), G(6.6743e-11) { }
#endif

void GravitationalForceSystem::update(Context &context, EntityId id, real deltaTime) {
#ifdef USE_BARNES_HUT
    quadtree->addPoint(id, context);
#else
    auto &position1 = context.getComponent<LocationComponent>(id).linear;
    auto &velocity1 = context.getComponent<VelocityComponent>(id);
    auto &massInfo1 = context.getComponent<MassInfoComponent>(id);
    auto entitiesSize = context.getEntitiesSize();
    for (size_t i = id + 1; i < entitiesSize; ++i) {
        if(!context.checkEntity(i, componentsBitset)) continue;
        auto &position2 = context.getComponent<LocationComponent>(i).linear;
        auto &velocity2 = context.getComponent<VelocityComponent>(i);
        auto &massInfo2 = context.getComponent<MassInfoComponent>(i);
        auto displacement = position2 - position1;
        auto direction = displacement.getNormalized();
        auto force = G * massInfo1.mass * massInfo2.mass / displacement.getSqrMagnitude();
        velocity1.linear += direction * force * massInfo1.inverseMass * deltaTime;
        velocity2.linear -= direction * force * massInfo2.inverseMass * deltaTime;
    }
#endif
}

#ifdef USE_BARNES_HUT
void GravitationalForceSystem::update(Context &context, real deltaTime) {
#ifndef USE_QUADTREE_FOR_BROADPHASE
    if(iterationNumber >= hardClearPeriod) {
        quadtree->hardClear();
        iterationNumber = 0;
    } else {
        quadtree->softClear();
        iterationNumber++;
    }
#else
    quadtree->softClear();
#endif

    System::update(context, deltaTime);

    const auto sqrTheta = theta * theta;
    const auto constG = G;
    const auto *qTree = quadtree;

    quadtree->forEachLeafWithMass([&context, qTree, deltaTime, sqrTheta, constG](const MultiBodyWithMass *body) {
        const auto centerOfMass1 = body->centerOfMass;
        const auto &ids = body->getIds();
        if(ids.size() > 1) {
            for (auto it1 = ids.begin(); it1 != ids.end(); ++it1) {
                auto it2 = it1;
                auto e1 = *it1;
                auto &position1 = context.getComponent<LocationComponent>(e1).linear;
                auto &velocity1 = context.getComponent<VelocityComponent>(e1);
                auto &massInfo1 = context.getComponent<MassInfoComponent>(e1);
                for (++it2; it2 != ids.end(); ++it2) {
                    auto e2 = *it2;
                    auto &position2 = context.getComponent<LocationComponent>(e2).linear;
                    auto &velocity2 = context.getComponent<VelocityComponent>(e2);
                    auto &massInfo2 = context.getComponent<MassInfoComponent>(e2);
                    auto displacement = position2 - position1;
                    auto direction = displacement.getNormalized();
                    auto force = constG * massInfo1.mass * massInfo2.mass / displacement.getSqrMagnitude();
                    velocity1.linear += direction * force * massInfo1.inverseMass * deltaTime;
                    velocity2.linear -= direction * force * massInfo2.inverseMass * deltaTime;
                }
            }
        }
        qTree->forFirstNodesWithMassWhere([&context, &ids, deltaTime, constG](const MultiBodyWithMass *secondBody) {
            for(const auto id : ids) {
                const auto &position1 = context.getComponent<LocationComponent>(id).linear;
                auto &velocity1 = context.getComponent<VelocityComponent>(id);

                auto &position2 = secondBody->centerOfMass;
                auto mass2 = secondBody->mass;
                auto displacement = position2 - position1;
                auto direction = displacement.getNormalized();
                auto acceleration = constG * mass2 / displacement.getSqrMagnitude();
                velocity1.linear += direction * acceleration * deltaTime;
            }
        },
[centerOfMass1, sqrTheta](const MultiBodyWithMass *secondBody) {
            if(secondBody->centerOfMass == centerOfMass1) return false;
            if(secondBody->isLeaf()) return true;
            const auto sqrSize = secondBody->getSize().getSqrMagnitude();
            const auto sqrDisplacement = (secondBody->centerOfMass - centerOfMass1).getSqrMagnitude();
            return sqrSize / sqrDisplacement < sqrTheta;
        });
    });
}
#endif


#ifndef ADJUSTABLEPHYSICS2D_GRAVITATIONALFORCESYSTEM_H
#define ADJUSTABLEPHYSICS2D_GRAVITATIONALFORCESYSTEM_H

#include "System.h"

class GravitationalForceSystem : public System {
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    GravitationalForceSystem();
    real G;
};

#endif //ADJUSTABLEPHYSICS2D_GRAVITATIONALFORCESYSTEM_H

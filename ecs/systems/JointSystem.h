#ifndef ADJUSTABLEPHYSICS2D_JOINTSYSTEM_H
#define ADJUSTABLEPHYSICS2D_JOINTSYSTEM_H

#include "System.h"

class JointSystem : public System {
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    JointSystem();
    void update(Context &context, real deltaTime) override;
};

#endif //ADJUSTABLEPHYSICS2D_JOINTSYSTEM_H

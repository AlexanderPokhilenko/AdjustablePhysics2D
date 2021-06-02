#ifndef ADJUSTABLEPHYSICS2D_CONSTRAINTSYSTEM_H
#define ADJUSTABLEPHYSICS2D_CONSTRAINTSYSTEM_H

#include "System.h"

class ConstraintSystem : public System {
private:
    static ComponentsBitset createCurrentSystemBitset();
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    ConstraintSystem();
};

#endif //ADJUSTABLEPHYSICS2D_CONSTRAINTSYSTEM_H

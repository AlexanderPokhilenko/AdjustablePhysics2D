#ifndef ADJUSTABLEPHYSICS2D_MOVESYSTEM_H
#define ADJUSTABLEPHYSICS2D_MOVESYSTEM_H

#include "System.h"

class MoveSystem : public System {
private:
    static ComponentsBitset createCurrentSystemBitset();
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    MoveSystem();
};

#endif //ADJUSTABLEPHYSICS2D_MOVESYSTEM_H

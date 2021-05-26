#ifndef ADJUSTABLEPHYSICS2D_GLOBALGRAVITATIONSYSTEM_H
#define ADJUSTABLEPHYSICS2D_GLOBALGRAVITATIONSYSTEM_H

#include "System.h"

class GlobalGravitationSystem : public System {
private:
    static ComponentsBitset createCurrentSystemBitset();
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    GlobalGravitationSystem();
    Vector2 g;
};

#endif //ADJUSTABLEPHYSICS2D_GLOBALGRAVITATIONSYSTEM_H

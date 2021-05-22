#ifndef ADJUSTABLEPHYSICS2D_SYSTEM_H
#define ADJUSTABLEPHYSICS2D_SYSTEM_H

#include "../Context.h"

class System {
protected:
    const ComponentsBitset componentsBitset;
    virtual void update(Context& context, EntityId id, real deltaTime) = 0;
public:
    explicit System(ComponentsBitset componentsBitset = ComponentsBitset());
    virtual void update(Context& context, real deltaTime);
    virtual ~System() = default;
};

#endif //ADJUSTABLEPHYSICS2D_SYSTEM_H

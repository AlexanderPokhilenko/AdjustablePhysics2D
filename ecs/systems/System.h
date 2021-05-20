#ifndef ADJUSTABLEPHYSICS2D_SYSTEM_H
#define ADJUSTABLEPHYSICS2D_SYSTEM_H

#include "../Context.h"

class System {
private:
    const ComponentsBitset componentsBitset;
protected:
    virtual void update(Context& context, EntityId id, real deltaTime) = 0;
public:
    explicit System(ComponentsBitset componentsBitset = ComponentsBitset());
    void update(Context& context, real deltaTime);
    virtual ~System() = 0;
};
#endif //ADJUSTABLEPHYSICS2D_SYSTEM_H

#ifndef ADJUSTABLEPHYSICS2D_COLLISION_H
#define ADJUSTABLEPHYSICS2D_COLLISION_H

#include "../math/Vector2.h"

struct Collision {
    EntityId first;
    EntityId second;
    real penetration;
    Vector2 normal;
};

#endif //ADJUSTABLEPHYSICS2D_COLLISION_H

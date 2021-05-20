#ifndef ADJUSTABLEPHYSICS2D_MATERIAL_H
#define ADJUSTABLEPHYSICS2D_MATERIAL_H

#include "../base/primitives.h"

struct Material {
#ifdef USE_DENSITY
    real density;
#endif
#ifdef USE_FRICTION
    real friction;
#endif
#ifdef USE_BOUNCINESS
    real bounciness;
#endif
};

#endif //ADJUSTABLEPHYSICS2D_MATERIAL_H

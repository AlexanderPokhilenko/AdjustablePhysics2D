#ifndef ADJUSTABLEPHYSICS2D_MASSINFO_H
#define ADJUSTABLEPHYSICS2D_MASSINFO_H

#include "../base/primitives.h"

struct MassInfo {
    real mass;
    real inverseMass;
#ifdef USE_INERTIA
    real inertia;
    real inverseInertia;
#endif
};

#endif //ADJUSTABLEPHYSICS2D_MASSINFO_H

#ifndef ADJUSTABLEPHYSICS2D_MASSINFO_H
#define ADJUSTABLEPHYSICS2D_MASSINFO_H

#include "../base/primitives.h"

struct MassInfo {
#ifndef USE_EQUAL_MASSES
    real mass;
    real inverseMass;
#else
#ifndef MASS
#define MASS 1
#endif
    static constexpr real mass = real(MASS);
    static constexpr real inverseMass = 1 / mass;
#endif
#ifdef USE_INERTIA
    real inertia;
    real inverseInertia;
#endif
};

#endif //ADJUSTABLEPHYSICS2D_MASSINFO_H

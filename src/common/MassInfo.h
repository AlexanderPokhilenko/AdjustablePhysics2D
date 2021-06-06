#ifndef ADJUSTABLEPHYSICS2D_MASSINFO_H
#define ADJUSTABLEPHYSICS2D_MASSINFO_H

#include "../base/primitives.h"

struct MassInfo {
#ifndef USE_EQUAL_MASSES
    real mass;
    real inverseMass;
#ifndef USE_INERTIA
    MassInfo(real mass);
#endif
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
#ifndef USE_EQUAL_MASSES
    MassInfo(real mass, real inertia);
#else
    MassInfo(real inertia);
#endif
#endif
    MassInfo();
};

#endif //ADJUSTABLEPHYSICS2D_MASSINFO_H

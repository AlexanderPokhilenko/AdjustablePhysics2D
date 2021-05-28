#include "MassInfo.h"

#if !defined(USE_EQUAL_MASSES) && !defined(USE_INERTIA)
MassInfo::MassInfo(real mass) {
    this->mass = mass;
    inverseMass = real(1) / mass;
}
#endif

MassInfo::MassInfo()
#ifndef USE_EQUAL_MASSES
: mass(1), inverseMass(1)
#ifdef USE_INERTIA
, inertia(1), inverseInertia(1)
#endif
#else
: inertia(1), inverseInertia(1)
#endif
{ }

#ifdef USE_INERTIA
#ifndef USE_EQUAL_MASSES
MassInfo::MassInfo(real mass, real inertia) {
    this->mass = mass;
    inverseMass = real(1) / mass;
    this->inertia = inertia;
    inverseInertia = real(1) / inertia;
}
#else
MassInfo::MassInfo(real inertia) {
    this->inertia = inertia;
    inverseInertia = real(1) / inertia;
}
#endif
#endif

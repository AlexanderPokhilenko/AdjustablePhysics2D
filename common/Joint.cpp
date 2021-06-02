#include "Joint.h"

Joint::Joint(EntityId id1, EntityId id2, real kSpring, real kDamper, real length, Vector2 point1, Vector2 point2) :
    id1(id1), id2(id2), kSpring(kSpring), kDamper(kDamper), length(length), point1(point1), point2(point2) { }

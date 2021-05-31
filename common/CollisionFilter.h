#ifndef ADJUSTABLEPHYSICS2D_COLLISIONFILTER_H
#define ADJUSTABLEPHYSICS2D_COLLISIONFILTER_H

#include <bitset>

#if !defined(COLLISION_CATEGORIES) || COLLISION_CATEGORIES < 1
#define COLLISION_CATEGORIES 8
#endif

struct CollisionFilter {
    std::bitset<COLLISION_CATEGORIES> category;
    std::bitset<COLLISION_CATEGORIES> mask;
    CollisionFilter();
    CollisionFilter(std::bitset<COLLISION_CATEGORIES> category, std::bitset<COLLISION_CATEGORIES> mask);
    bool check(const CollisionFilter &second) const;
};

#endif //ADJUSTABLEPHYSICS2D_COLLISIONFILTER_H

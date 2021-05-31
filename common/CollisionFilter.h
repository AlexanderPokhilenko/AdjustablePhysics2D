#ifndef ADJUSTABLEPHYSICS2D_COLLISIONFILTER_H
#define ADJUSTABLEPHYSICS2D_COLLISIONFILTER_H

#include <bitset>

#if !defined(COLLISION_CATEGORIES) || COLLISION_CATEGORIES < 1
#define COLLISION_CATEGORIES 8
#endif

using CollisionFilterBitset = std::bitset<COLLISION_CATEGORIES>;

struct CollisionFilter {
    CollisionFilterBitset category;
    CollisionFilterBitset mask;
    CollisionFilter();
    CollisionFilter(CollisionFilterBitset category);
    CollisionFilter(CollisionFilterBitset category, CollisionFilterBitset mask);
    bool check(const CollisionFilter &second) const;
};

#endif //ADJUSTABLEPHYSICS2D_COLLISIONFILTER_H

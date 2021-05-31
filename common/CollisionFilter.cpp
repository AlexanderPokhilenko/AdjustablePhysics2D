#include "CollisionFilter.h"

CollisionFilter::CollisionFilter() : category(), mask() {
    category.set(0);
    mask.set();
}

CollisionFilter::CollisionFilter(std::bitset<8> category, std::bitset<8> mask) : category(category), mask(mask) { }

bool CollisionFilter::check(const CollisionFilter &second) const {
    return (category & second.mask).any() && (second.category & mask).any();
}

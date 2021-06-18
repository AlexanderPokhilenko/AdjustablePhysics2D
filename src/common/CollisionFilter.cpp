#include "CollisionFilter.h"

CollisionFilter::CollisionFilter() : category(), mask() {
    category.set(0);
    mask.set();
}

CollisionFilter::CollisionFilter(CollisionFilterBitset category) : category(category), mask() {
    mask.set();
}

CollisionFilter::CollisionFilter(CollisionFilterBitset category, CollisionFilterBitset mask) : category(category), mask(mask) { }

bool CollisionFilter::check(const CollisionFilter &second) const {
    return (category & second.mask).any() && (second.category & mask).any();
}

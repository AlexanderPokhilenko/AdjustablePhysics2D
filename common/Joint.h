#ifndef ADJUSTABLEPHYSICS2D_JOINT_H
#define ADJUSTABLEPHYSICS2D_JOINT_H

#include "../base/primitives.h"
#include "../math/Vector2.h"
#include <functional>

struct Joint {
    EntityId id1;
    EntityId id2;
    real kSpring;
    real kDamper;
    real length;
    Vector2 point1;
    Vector2 point2;
    Joint(EntityId id1, EntityId id2, real kSpring, real kDamper = 0, real length = 0, Vector2 point1 = Vector2(), Vector2 point2 = Vector2());
};

struct JointHash {
    inline std::size_t operator()(const Joint &j) const {
        auto hEId = std::hash<EntityId>{};
        auto hR = std::hash<real>{};
        auto hV2 = Vector2Hash{};
        return hEId(j.id1) ^ (hEId(j.id2) << 1) ^ (hR(j.kSpring) << 2) ^ (hR(j.kDamper) << 3) ^ (hR(j.length) << 4) ^ (hV2(j.point1) << 5) ^ (hV2(j.point2) << 6);
    }
};

#endif //ADJUSTABLEPHYSICS2D_JOINT_H

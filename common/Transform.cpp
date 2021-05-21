# include "Transform.h"

Transform Transform::operator-() const {
    return { -linear
#if USE_ROTATION
            , -angular
#endif
    };
}

Transform Transform::operator+(const Transform& second) const {
    return { linear + second.linear
#if USE_ROTATION
            , angular + second.angular
#endif
    };
}

Transform Transform::operator-(const Transform& second) const {
    return { linear - second.linear
#if USE_ROTATION
            , angular - second.angular
#endif
    };
}

Transform Transform::operator*(real k) const {
    return { linear * k
#if USE_ROTATION
            , angular * k
#endif
    };
}

Transform Transform::operator/(real k) const {
    return { linear / k
#if USE_ROTATION
            , angular / k
#endif
    };
}

Transform &Transform::operator+=(const Transform &right) {
    linear += right.linear;
#ifdef USE_ROTATION
    angular += right.angular;
#endif
    return *this;
}

Transform &Transform::operator-=(const Transform &right) {
    linear -= right.linear;
#ifdef USE_ROTATION
    angular -= right.angular;
#endif
    return *this;
}

# include "Transform.h"

#ifdef USE_ROTATION
Transform::Transform(Vector2 lin, real ang) : linear(lin), angular(ang) {  }
Transform::Transform(Vector2 lin) : linear(lin), angular(0) {  }
Transform::Transform() : linear(Vector2()), angular(0) {  }
#else
Transform::Transform(Vector2 lin) : linear(lin) {  }
Transform::Transform() : linear(Vector2()) {  }
#endif

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

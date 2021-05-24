#include "Vector2.h"
#include <cmath>

real Vector2::dotProduct(const Vector2 &first, const Vector2 &second) {
    return first.x * second.x + first.y * second.y;
}

real Vector2::crossProduct(const Vector2 &first, const Vector2 &second) {
    return first.x * second.y - first.y * second.x;
}

real Vector2::getSqrMagnitude() const {
    return x * x + y * y;
}

real Vector2::getMagnitude() const {
#ifdef DOUBLE_PRECISION
    return sqrt(getSqrMagnitude());
#else
    return sqrtf(getSqrMagnitude());
#endif
}

Vector2 Vector2::getNormalized() const {
    auto magnitude = getMagnitude();
    auto newX = x / magnitude;
    auto newY = y / magnitude;
    return {newX, newY};
}

void Vector2::normalize() {
    auto magnitude = getMagnitude();
    x /= magnitude;
    y /= magnitude;
}

Vector2 Vector2::getRotated(real angle) const {
    real sinVal, cosVal;
#ifdef DOUBLE_PRECISION
    sinVal = sin(angle);
    cosVal = cos(angle);
#else
    sinVal = sinf(angle);
    cosVal = cosf(angle);
#endif
    return {
        x * cosVal - y * sinVal,
        x * sinVal + y * cosVal
    };
}

void Vector2::rotate(real angle) {
    real sinVal, cosVal;
#ifdef DOUBLE_PRECISION
    sinVal = sin(angle);
    cosVal = cos(angle);
#else
    sinVal = sinf(angle);
    cosVal = cosf(angle);
#endif
    real newX = x * cosVal - y * sinVal;
    real newY = x * sinVal + y * cosVal;

    x = newX;
    y = newY;
}

Vector2 Vector2::operator+(const Vector2& second) const {
    return {x + second.x, y + second.y};
}

Vector2 Vector2::operator-(const Vector2& second) const {
    return {x - second.x, y - second.y};
}

Vector2 Vector2::operator*(real k) const {
    return {x * k, y * k};
}

Vector2 Vector2::operator/(real k) const {
    return {x / k, y / k};
}

Vector2 Vector2::operator-() const {
    return {-x, -y};
}

bool Vector2::operator==(const Vector2 &right) const {
    return x == right.x && y == right.y;
}

Vector2& Vector2::operator+=(const Vector2& right) {
    x += right.x;
    y += right.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& right) {
    x -= right.x;
    y -= right.y;
    return *this;
}

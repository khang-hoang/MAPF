#include "TypeDefine.hpp"

double Vector2d::length() const {
    return sqrt(x*x +y*y);
}

Vector2d operator +(const Vector2d& vector, const Vector2d& other) {
    return Vector2d(vector.x + other.x, vector.y + other.y);
}

Vector2d operator -(const Vector2d& vector) {
    return Vector2d(-vector.x, -vector.y);
}

Vector2d operator -(const Vector2d& vector, const Vector2d& other) {
    return Vector2d(vector.x - other.x, vector.y - other.y);
}

Vector2d operator *(const Vector2d& vector, const double& val) {
    return Vector2d(vector.x*val, vector.y*val);
}

Vector2d operator *(const double& val, const Vector2d& vector) {
    return Vector2d(vector.x*val, vector.y*val);
}

double dot_product(const Vector2d& v0, const Vector2d &v1) {
    return v0.x*v1.x + v0.y*v1.y;
}

double angle_rad(const Vector2d& v0, const Vector2d &v1) {
    double val = dot_product(v0,v1)/(v0.length()*v1.length());
    return acos(val);
}

#include "TypeDefine.hpp"

Vector2d operator +(const Vector2d& vector, const Vector2d& other) {
    return Vector2d(vector.x + other.x, vector.y + other.y);
}

Vector2d operator -(const Vector2d& vector) {
    return Vector2d(-vector.x, -vector.y);
}

Vector2d operator -(const Vector2d& vector, const Vector2d& other) {
    return Vector2d(vector.x - other.x, vector.y - other.y);
}

Vector2d operator *(const Vector2d& vector, const coordinate_type& val) {
    return Vector2d(vector.x*val, vector.y*val);
}

Vector2d operator *(const coordinate_type& val, const Vector2d& vector) {
    return Vector2d(vector.x*val, vector.y*val);
}

coordinate_type dot_product(const Vector2d& v0, const Vector2d &v1) {
    return v0.x*v1.x + v0.y*v1.y;
}

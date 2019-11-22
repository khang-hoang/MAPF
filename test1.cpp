#include <vector>
#include <math.h>
#include <iostream>

struct Vector2d {
    double x;
    double y;
    Vector2d(double t_x, double t_y) : x(t_x), y(t_y) {}
    Vector2d(): x(0), y(0) {}
    double length() const;
};

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

int main() {
    Vector2d v0(0,100);
    Vector2d v1(100,0);
    double rad = angle_rad(v0, v1);
    std::cout << rad << std::endl;
    return 0;
}

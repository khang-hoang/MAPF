#ifndef __TYPEDEFINE_HPP__
#define __TYPEDEFINE_HPP__

#include <boost/polygon/polygon.hpp>
#include <boost/polygon/voronoi.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <iostream>

#define PI 3.14159265359
#define HALF_PI 1.57079632679

typedef double coordinate_type;

struct Vector2d : public boost::geometry::model::point<double,2,boost::geometry::cs::cartesian> {
    double x;
    double y;
    Vector2d(double t_x, double t_y) : x(t_x), y(t_y) {}
    Vector2d(): x(0), y(0) {}
    double length() const;
};

Vector2d operator +(const Vector2d& vector, const Vector2d& other);
Vector2d operator -(const Vector2d& vector);
Vector2d operator -(const Vector2d& vector, const Vector2d& other);
Vector2d operator *(const Vector2d& vector, const double& val);
Vector2d operator *(const double& val, const Vector2d& vector);
double angle_rad(const Vector2d& v0, const Vector2d &v1);
double dot_product(const Vector2d& v0, const Vector2d &v1);

typedef Vector2d Point;

struct Segment {
    Point p0;
    Point p1;
    Segment(const Point& t_p0, const Point& t_p1) : p0(t_p0), p1(t_p1) {}
};

//First we register it as a point with boost polygon
namespace boost {
namespace polygon {
template <>
struct geometry_concept<Point> { typedef point_concept type; };

//Then we specialize the gtl point traits for our point type
template <>
struct point_traits<Point> {
    typedef int32_t coordinate_type;
    static inline coordinate_type get(const Point& point, orientation_2d orient) {
        if (orient == HORIZONTAL)
            return point.x;
        return point.y;
    }
};

template <>
struct point_mutable_traits<Point> {
    static inline void set(Point& point, orientation_2d orient, int value) {
        if (orient == HORIZONTAL)
            point.x = value;
        else
            point.y = value;
    }
    static inline Point construct(int x_value, int y_value) {
        return Point(x_value, y_value);
    }
};

template <>
struct geometry_concept<Segment> { typedef segment_concept type; };

template <>
struct segment_traits<Segment> {
    typedef Point point_type;

    static inline point_type get(const Segment& segment, direction_1d dir) {
        return dir.to_int() ? segment.p1 : segment.p0;
    }
};

template <>
struct segment_mutable_traits<Segment> {
    typedef typename segment_traits<Segment>::point_type point_type;

    static inline void set(Segment& segment, direction_1d dir, const point_type& point) {
        if (dir.to_int()) {
            segment.p1 = point;
        } else {
            segment.p0 = point;
        }
    }

    static inline Segment construct(const point_type& low, const point_type& high) {
        return Segment(low, high);
    }
};

}  // namespace polygon
}  // namespace boost

#endif  // __TYPEDEFINE_HPP__

#ifndef __TYPEDEFINE_HPP__
#define __TYPEDEFINE_HPP__

#include <boost/polygon/polygon.hpp>
#include <boost/polygon/voronoi.hpp>

typedef double coordinate_type;
typedef boost::polygon::voronoi_diagram<double> VoronoiDiagram;
typedef VoronoiDiagram::cell_type VoronoiCell;
typedef VoronoiDiagram::edge_type VoronoiEdge;
typedef VoronoiDiagram::vertex_type VoronoiVertex;

struct Point {
    coordinate_type x;
    coordinate_type y;
    Point(coordinate_type t_x, coordinate_type t_y) : x(t_x), y(t_y) {}
};

struct Segment {
    Point p0;
    Point p1;
    Segment(const Point &t_p0, const Point &t_p1) : p0(t_p0), p1(t_p1) {}
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

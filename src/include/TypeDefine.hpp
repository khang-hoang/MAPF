#ifndef __TYPEDEFINE_HPP__
#define __TYPEDEFINE_HPP__

#include <boost/polygon/polygon.hpp>
#include <boost/polygon/voronoi.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <iostream>

typedef double coordinate_type;

struct Vector2d {
    coordinate_type x;
    coordinate_type y;
    Vector2d(coordinate_type t_x, coordinate_type t_y) : x(t_x), y(t_y) {}
    Vector2d(): x(0), y(0) {}
};

typedef Vector2d Point;
Vector2d operator +(const Vector2d& vector, const Vector2d& other);
Vector2d operator -(const Vector2d& vector);
Vector2d operator -(const Vector2d& vector, const Vector2d& other);
Vector2d operator *(const Vector2d& vector, const coordinate_type& val);
Vector2d operator *(const coordinate_type& val, const Vector2d& vector);
coordinate_type dot_product(const Vector2d& v0, const Vector2d &v1);

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

// Forward declarations.
class VoronoiEdge;

// Represents Voronoi cell.
// Data members:
//   1) index of the source within the initial input set
//   2) pointer to the incident edge
//   3) mutable color member
// Cell may contain point or segment site inside.
class VoronoiCell {
   public:
    typedef coordinate_type coordinate_type;
    typedef std::size_t color_type;
    typedef VoronoiEdge voronoi_edge_type;
    typedef std::size_t source_index_type;
    typedef boost::polygon::SourceCategory source_category_type;

    VoronoiCell(source_index_type source_index,
                source_category_type source_category) : source_index_(source_index),
                                                        incident_edge_(NULL),
                                                        color_(source_category) {}

    // Returns true if the cell contains point site, false else.
    bool contains_point() const {
        source_category_type source_category = this->source_category();
        return belongs(source_category, boost::polygon::GEOMETRY_CATEGORY_POINT);
    }

    // Returns true if the cell contains segment site, false else.
    bool contains_segment() const {
        source_category_type source_category = this->source_category();
        return belongs(source_category, boost::polygon::GEOMETRY_CATEGORY_SEGMENT);
    }

    source_index_type source_index() const {
        return source_index_;
    }

    source_category_type source_category() const {
        return static_cast<source_category_type>(color_ & boost::polygon::SOURCE_CATEGORY_BITMASK);
    }

    // Degenerate cells don't have any incident edges.
    bool is_degenerate() const { return incident_edge_ == NULL; }

    voronoi_edge_type* incident_edge() { return incident_edge_; }
    const voronoi_edge_type* incident_edge() const { return incident_edge_; }
    void incident_edge(voronoi_edge_type* e) { incident_edge_ = e; }

    color_type color() const { return color_ >> BITS_SHIFT; }
    void color(color_type color) const {
        color_ &= BITS_MASK;
        color_ |= color << BITS_SHIFT;
    }

    int32_t idx;

   private:
    // 5 color bits are reserved.
    enum Bits {
        BITS_SHIFT = 0x5,
        BITS_MASK = 0x1F
    };

    source_index_type source_index_;
    voronoi_edge_type* incident_edge_;
    mutable color_type color_;
};

// Represents Voronoi vertex.
// Data members:
//   1) vertex coordinates
//   2) pointer to the incident edge
//   3) mutable color member
class VoronoiVertex {
   public:
    typedef coordinate_type coordinate_type;
    typedef std::size_t color_type;
    typedef VoronoiEdge voronoi_edge_type;

    VoronoiVertex(const coordinate_type& x, const coordinate_type& y) : x_(x),
                                                                        y_(y),
                                                                        incident_edge_(NULL),
                                                                        color_(0) {}

    const coordinate_type& x() const { return x_; }
    const coordinate_type& y() const { return y_; }

    bool is_degenerate() const { return incident_edge_ == NULL; }

    voronoi_edge_type* incident_edge() { return incident_edge_; }
    const voronoi_edge_type* incident_edge() const { return incident_edge_; }
    void incident_edge(voronoi_edge_type* e) { incident_edge_ = e; }

    color_type color() const { return color_ >> BITS_SHIFT; }
    void color(color_type color) const {
        color_ &= BITS_MASK;
        color_ |= color << BITS_SHIFT;
    }

    int32_t index;
   private:
    // 5 color bits are reserved.
    enum Bits {
        BITS_SHIFT = 0x5,
        BITS_MASK = 0x1F
    };

    coordinate_type x_;
    coordinate_type y_;
    voronoi_edge_type* incident_edge_;
    mutable color_type color_;
};

// Half-edge data structure. Represents Voronoi edge.
// Data members:
//   1) pointer to the corresponding cell
//   2) pointer to the vertex that is the starting
//      point of the half-edge
//   3) pointer to the twin edge
//   4) pointer to the CCW next edge
//   5) pointer to the CCW prev edge
//   6) mutable color member
class VoronoiEdge {
   public:
    typedef coordinate_type coordinate_type;
    typedef VoronoiCell voronoi_cell_type;
    typedef VoronoiVertex voronoi_vertex_type;
    typedef VoronoiEdge voronoi_edge_type;
    typedef std::size_t color_type;

    VoronoiEdge(bool is_linear, bool is_primary) : cell_(NULL),
                                                   vertex_(NULL),
                                                   twin_(NULL),
                                                   next_(NULL),
                                                   prev_(NULL),
                                                   color_(0) {
        if (is_linear)
            color_ |= BIT_IS_LINEAR;
        if (is_primary)
            color_ |= BIT_IS_PRIMARY;
    }

    voronoi_cell_type* cell() { return cell_; }
    const voronoi_cell_type* cell() const { return cell_; }
    void cell(voronoi_cell_type* c) { cell_ = c; }

    voronoi_vertex_type* vertex0() { return vertex_; }
    const voronoi_vertex_type* vertex0() const { return vertex_; }
    void vertex0(voronoi_vertex_type* v) { vertex_ = v; }

    voronoi_vertex_type* vertex1() { return twin_->vertex0(); }
    const voronoi_vertex_type* vertex1() const { return twin_->vertex0(); }

    voronoi_edge_type* twin() { return twin_; }
    const voronoi_edge_type* twin() const { return twin_; }
    void twin(voronoi_edge_type* e) { twin_ = e; }

    voronoi_edge_type* next() { return next_; }
    const voronoi_edge_type* next() const { return next_; }
    void next(voronoi_edge_type* e) { next_ = e; }

    voronoi_edge_type* prev() { return prev_; }
    const voronoi_edge_type* prev() const { return prev_; }
    void prev(voronoi_edge_type* e) { prev_ = e; }

    // Returns a pointer to the rotation next edge
    // over the starting point of the half-edge.
    voronoi_edge_type* rot_next() { return prev_->twin(); }
    const voronoi_edge_type* rot_next() const { return prev_->twin(); }

    // Returns a pointer to the rotation prev edge
    // over the starting point of the half-edge.
    voronoi_edge_type* rot_prev() { return twin_->next(); }
    const voronoi_edge_type* rot_prev() const { return twin_->next(); }

    // Returns true if the edge is finite (segment, parabolic arc).
    // Returns false if the edge is infinite (ray, line).
    bool is_finite() const { return vertex0() && vertex1(); }

    // Returns true if the edge is infinite (ray, line).
    // Returns false if the edge is finite (segment, parabolic arc).
    bool is_infinite() const { return !vertex0() || !vertex1(); }

    // Returns true if the edge is linear (segment, ray, line).
    // Returns false if the edge is curved (parabolic arc).
    bool is_linear() const {
        return (color_ & BIT_IS_LINEAR) ? true : false;
    }

    // Returns true if the edge is curved (parabolic arc).
    // Returns false if the edge is linear (segment, ray, line).
    bool is_curved() const {
        return (color_ & BIT_IS_LINEAR) ? false : true;
    }

    // Returns false if edge goes through the endpoint of the segment.
    // Returns true else.
    bool is_primary() const {
        return (color_ & BIT_IS_PRIMARY) ? true : false;
    }

    // Returns true if edge goes through the endpoint of the segment.
    // Returns false else.
    bool is_secondary() const {
        return (color_ & BIT_IS_PRIMARY) ? false : true;
    }

    color_type color() const { return color_ >> BITS_SHIFT; }
    void color(color_type color) const {
        color_ &= BITS_MASK;
        color_ |= color << BITS_SHIFT;
    }

    void set_secondary() {
        const VoronoiVertex *v0 = this->vertex0();
        const VoronoiVertex *v1 = this->vertex1();
        color_ &= BITS_MASK;
        color_ &= 0x1 << BITS_SHIFT;
    }

    int32_t id;

   private:
    // 5 color bits are reserved.
    enum Bits {
        BIT_IS_LINEAR = 0x1,   // linear is opposite to curved
        BIT_IS_PRIMARY = 0x2,  // primary is opposite to secondary

        BITS_SHIFT = 0x5,
        BITS_MASK = 0x1F
    };

    voronoi_cell_type* cell_;
    voronoi_vertex_type* vertex_;
    voronoi_edge_type* twin_;
    voronoi_edge_type* next_;
    voronoi_edge_type* prev_;
    mutable color_type color_;
};

struct my_voronoi_diagram_traits {
    typedef coordinate_type coordinate_type;
    typedef VoronoiCell cell_type;
    typedef VoronoiVertex vertex_type;
    typedef VoronoiEdge edge_type;
    typedef boost::polygon::detail::int32 int_type;
    typedef boost::polygon::detail::int64 int_x2_type;
    typedef boost::polygon::detail::uint64 uint_x2_type;
    typedef boost::polygon::detail::extended_int<64> big_int_type;
    typedef boost::polygon::detail::fpt64 fpt_type;
    typedef boost::polygon::detail::extended_exponent_fpt<fpt_type> efpt_type;
    typedef boost::polygon::detail::ulp_comparison<fpt_type> ulp_cmp_type;
    typedef boost::polygon::detail::type_converter_fpt to_fpt_converter_type;
    typedef boost::polygon::detail::type_converter_efpt to_efpt_converter_type;
    typedef class {
       public:
        enum { ULPS = 128 };
        bool operator()(const vertex_type& v1, const vertex_type& v2) const {
            return (ulp_cmp(v1.x(), v2.x(), ULPS) ==
                    boost::polygon::detail::ulp_comparison<coordinate_type>::EQUAL) &&
                   (ulp_cmp(v1.y(), v2.y(), ULPS) ==
                    boost::polygon::detail::ulp_comparison<coordinate_type>::EQUAL);
        }

       private:
        typename boost::polygon::detail::ulp_comparison<coordinate_type> ulp_cmp;
    } vertex_equality_predicate_type;
};

typedef boost::polygon::voronoi_diagram<double, my_voronoi_diagram_traits> VoronoiDiagram;
typedef boost::polygon::voronoi_builder<double, my_voronoi_diagram_traits> VoronoiBuilder;

#endif  // __TYPEDEFINE_HPP__

//
//  VDMap.cpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#include "Map.hpp"
#include "VDMap.hpp"
#include <fstream>
#include <unordered_map>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>

typedef boost::polygon::voronoi_edge<double> VoronoiEdge;
typedef boost::polygon::voronoi_vertex<double> VoronoiVertex;
typedef boost::polygon::voronoi_cell<double> VoronoiCell;
typedef boost::polygon::voronoi_diagram<double> VoronoiDiagram;

VDMap::VDMap(int32_t t_width, int32_t t_height) : Map(t_width, t_height) {
    this->m_rtree = new rtree;
}

VDMap::~VDMap() {
    // std::cout << "~Map" << std::endl;
    // for (Obstacle *obs : this->m_list_obstacle) {
    //     delete obs;
    // }
    // delete this->m_graph;
}

void VDMap::constructGraph() {
    std::vector<Point> listPoint;
    std::vector<Segment> listSegment;
    VoronoiDiagram vd;
    for (Vertex* v : this->m_list_vertex) {
        delete v;
    }
    this->m_list_vertex.clear();
    Point p0 = Point(0, 0);
    Point p1 = Point(this->m_width, 0);
    Point p2 = Point(this->m_width, this->m_height);
    Point p3 = Point(0, this->m_height);
    // add boundingbox line segments
    listSegment.push_back(Segment(p0,p1));
    listSegment.push_back(Segment(p1,p2));
    listSegment.push_back(Segment(p2,p3));
    listSegment.push_back(Segment(p3,p0));
    for (Obstacle *obs : this->m_listObstacle) {
        std::vector<Point>::iterator it0 = std::prev(obs->listPoint.end());
        std::vector<Point>::iterator it1 = obs->listPoint.begin();
        for (; it1 != obs->listPoint.end(); it0 = it1, it1++) {
            listPoint.push_back(*it1);
            listSegment.push_back(Segment(Point(it0->x, it0->y),Point(it1->x,it1->y)));
        }
    }
    boost::polygon::construct_voronoi(listSegment.begin(), listSegment.end(), &vd);

    // make list of Graph vertex and eliminate in-obstacle VoronoiVertex
    std::unordered_map<VoronoiVertex*,Vertex*> map_vertex;
    std::vector<VoronoiVertex> &list_vertex = const_cast<std::vector<VoronoiVertex>&>(vd.vertices());
    for (VoronoiVertex &v : list_vertex) {
        bool is_valid = true;
        for (const Obstacle *obs : this->m_listObstacle) {
            if (obs->contains(v.x(),v.y())) {
                is_valid = false;
                break;
            }
        }
        if (is_valid) {
            Vertex *vertex = new Vertex(v.x(), v.y());
            this->m_list_vertex.push_back(vertex);
            map_vertex.insert(std::make_pair(&v, vertex));
        }
    }
    // add nearest obstacle points to vertex
    std::vector<VoronoiEdge> &list_edge = const_cast<std::vector<VoronoiEdge>&>(vd.edges());
    for (VoronoiEdge &edge : list_edge) {
        if (edge.is_primary() && edge.is_finite()) {
            VoronoiVertex *v0 = edge.vertex0();
            VoronoiVertex *v1 = edge.vertex1();
            auto it0 = map_vertex.find(v0);
            auto it1 = map_vertex.find(v1);
            if (it0 != map_vertex.end() && it1 != map_vertex.end()) {
                Vertex* vertex0 = it0->second;
                Vertex* vertex1 = it1->second;
                vertex0->neighbors.push_back(vertex1);

                auto cell = edge.cell();
                if (vertex0->obsPoints.size() < vertex0->neighbors.size()) {
                    Segment seg = listSegment[cell->source_index()];
                    Vector2d p1p0(seg.p1.x - seg.p0.x, seg.p1.y - seg.p0.y);
                    Vector2d v0p0(v0->x() - seg.p0.x, v0->y() - seg.p0.y);
                    double lambda0 = dot_product(v0p0,p1p0) / dot_product(p1p0,p1p0);
                    if (lambda0 <= 0) {
                        vertex0->addObsPoint(seg.p0);
                    } else if (lambda0 >= 1) {
                        vertex0->addObsPoint(seg.p1);
                    } else {
                        Vector2d s0 = Vector2d(seg.p0.x, seg.p0.y) + lambda0*p1p0;
                        vertex0->addObsPoint(Point(s0.x, s0.y));
                    }
                }
            }
        }
    }
    this->m_rtree->clear();
    typedef boost::geometry::model::multi_point<point> multi_point;
    int i = 0;
    for (Vertex* v : this->m_list_vertex) {
        for (Vertex *n : v->neighbors) {
            if (!n->visited) {
                multi_point mp;
                Vector2d vn(n->x() - v->x(), n->y() - v->y());
                for (Vertex *v0 : {v, n}) {
                    if (v0->obsPoints.size() > 1) {
                        Point &n0 = v0->obsPoints[0];
                        Point &n1 = v0->obsPoints[1];
                        if (v0->obsPoints.size() > 2) {
                            for (Point &p : v0->obsPoints) {
                                Vector2d v0p(p.x - v0->x(),p.y - v0->y());
                                double rad = angle_rad(v0p, v0 == v ? vn : -vn);
                                if (rad <= HALF_PI) {
                                    mp.push_back(point(p.x, p.y));
                                }
                            }
                        } else {
                            mp.push_back(point(n0.x, n0.y));
                            mp.push_back(point(n1.x, n1.y));
                        }
                    }
                    mp.push_back(point(v0->x(), v0->y()));
                }
                box b = boost::geometry::return_envelope<box>(mp);
                this->m_rtree->insert(std::make_pair(b, std::vector<Vertex*>({v,n})));
            }
        }
        v->visited = true;
    }
}

std::vector<Vertex*> VDMap::getNearestEdge(const Point& pos) {
    std::vector<value> result_n;
    this->m_rtree->query(boost::geometry::index::contains(point(pos.x, pos.y)), std::back_inserter(result_n));
    if (result_n.size() > 0) {
        return result_n.front().second;
    }
    return std::vector<Vertex*>();
}

//
//  Map.cpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#include "Map.hpp"
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>

typedef boost::polygon::voronoi_edge<double> VoronoiEdge;
typedef boost::polygon::voronoi_vertex<double> VoronoiVertex;
typedef boost::polygon::voronoi_cell<double> VoronoiCell;
typedef boost::polygon::voronoi_diagram<double> VoronoiDiagram;

Obstacle::Obstacle(std::vector<Point> t_listPoint) : listPoint(t_listPoint){};

void Obstacle::move(int32_t t_deltaX, int32_t t_deltaY) {
    for (Point &point : this->listPoint) {
        point.x += t_deltaX;
        point.y += t_deltaY;
    }
}

void Obstacle::removePoint(const Point &t_p) {
    std::vector<Point>::iterator it = this->listPoint.begin();
    for (;it != this->listPoint.end();it++) {
        Point &p = *it;
        if (p.x == t_p.x && p.y == t_p.y) {
            this->listPoint.erase(it);
            return;
        }
    }
}

bool Obstacle::contains(int32_t t_posX, int32_t t_posY) const {
    typedef boost::geometry::model::d2::point_xy<double> point_type;
    typedef boost::geometry::model::polygon<point_type> Polygon;
    Polygon poly;
    for (const Point &p : this->listPoint) {
        boost::geometry::append(poly.outer(), point_type(p.x, p.y));
    }
    const Point &pFirst = this->listPoint[0];
    boost::geometry::append(poly.outer(), point_type(pFirst.x, pFirst.y));
    return boost::geometry::within(point_type(t_posX, t_posY), poly);
}

Vertex::Vertex() : pos(0,0), visited(false) { }

Vertex::Vertex(double x, double y) : pos(x, y), visited(false) { }

Vertex::~Vertex() { }

double Vertex::x() const {
    return this->pos.x;
}

double Vertex::y() const {
    return this->pos.y;
}

void Vertex::setPos(double x, double y) {
    this->pos.x = x;
    this->pos.y = y;
}

void Vertex::addObsPoint(const Point& point) {
    for (const Point &p : this->obsPoints) {
        if (abs(p.x - point.x) < 0.1 && abs(p.y - point.y) < 0.1) {
            return;
        }
    }
    this->obsPoints.push_back(point);
}

Map::Map(int32_t t_width, int32_t t_height) : m_width(t_width), m_height(t_height) {
    this->m_rtree = new rtree;
}

Map::~Map() {
    // std::cout << "~Map" << std::endl;
    // for (Obstacle *obs : this->m_list_obstacle) {
    //     delete obs;
    // }
    // delete this->m_graph;
}

void Map::constructGraph() {
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

std::vector<Vertex*> Map::getNearestEdge(const Point& pos) {
    std::vector<value> result_n;
    this->m_rtree->query(boost::geometry::index::contains(point(pos.x, pos.y)), std::back_inserter(result_n));
    if (result_n.size() > 0) {
        return result_n.front().second;
    }
    return std::vector<Vertex*>();
}

void Map::saveTofile(const std::string &filename) {
    std::ofstream outFile(filename);
    outFile << this->m_width << ' ' << this->m_height << std::endl;
    for (Obstacle *obs : this->m_listObstacle) {
        for (Point &p : obs->listPoint) {
            outFile << '(' << p.x << ',' << p.y << ") ";
        }
        outFile << std::endl;
    }
    outFile.close();
}

Map Map::readFromFile(const std::string &filename) {
    std::ifstream inFile(filename);
    int32_t width;
    int32_t height;
    inFile >> width >> height;
    std::vector<Point> listPoint;
    Map map(width, height);
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));
            int32_t x, y;
            for (std::string &pair : tokens) {
                std::istringstream iss(pair);
                iss.ignore();
                iss >> x;
                if (!iss.bad() && iss.peek() == ',') {
                    iss.ignore();
                    iss >> y;
                    listPoint.push_back(Point(x, y));
                } else {
                    std::cout << "read file error." << std::endl;
                    throw;
                }
            }
            Obstacle *obs = map.addObstacle(std::vector<Point>(listPoint));
            listPoint.clear();
        }
    }
    return map;
}

Obstacle *Map::addObstacle(const std::vector<Point> &t_listPoint) {
    Obstacle *obs = new Obstacle(t_listPoint);
    this->m_listObstacle.push_back(obs);
    return obs;
}

void Map::removeObstacle(const Obstacle* t_obs) {
    std::vector<Obstacle*>::iterator it = this->m_listObstacle.begin();
    for (;it != this->m_listObstacle.end();it++) {
        if (*it == t_obs) {
            this->m_listObstacle.erase(it);
            return;
        }
    }
}

std::vector<Obstacle*> Map::getListObstacle() const {
    return m_listObstacle;
}

std::vector<Vertex*> Map::getGraph() const {
    return m_list_vertex;
}

std::vector<Vertex*> Map::cloneGraph() const {
    std::vector<Vertex*> list_vertex;
    std::unordered_map<Vertex*,Vertex*> map_vertex;
    for (Vertex *v : this->m_list_vertex) {
        Vertex *v0 = new Vertex(v->x(),v->y());
        v0->obsPoints = std::vector<Point>(v->obsPoints);
        list_vertex.push_back(v0);
        map_vertex.insert(std::make_pair(v, v0));
    }
    for (Vertex *v : this->m_list_vertex) {
        auto it0 = map_vertex.find(v);
        Vertex *v0 = it0->second;
        for (Vertex *n : v->neighbors) {
            auto it1 = map_vertex.find(n);
            Vertex *v1 = it1->second;
            v0->neighbors.push_back(v1);
        }
    }
    return list_vertex;
}

int32_t Map::getWidth() const {
    return m_width;
}

int32_t Map::getHeight() const {
    return m_height;
}

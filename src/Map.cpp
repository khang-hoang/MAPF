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

Obstacle::Obstacle(std::vector<Point> t_listPoint) : listPoint(t_listPoint){};

void Obstacle::move(int32_t t_deltaX, int32_t t_deltaY) {
    for (Point &point : this->listPoint) {
        point.x += t_deltaX;
        point.y += t_deltaY;
    }
}

bool Obstacle::contains(int32_t t_posX, int32_t t_posY) const {
    typedef boost::geometry::model::d2::point_xy<double> point_type;
    typedef boost::geometry::model::pointing_segment<double> segment_type;
    typedef boost::geometry::model::polygon<point_type> Polygon;
    Polygon poly;
    for (const Point &p : this->listPoint) {
        boost::geometry::append(poly.outer(), point_type(p.x, p.y));
    }
    const Point &pFirst = this->listPoint[0];
    boost::geometry::append(poly.outer(), point_type(pFirst.x, pFirst.y));
    return boost::geometry::covered_by(point_type(t_posX, t_posY), poly);
}

Vertex::Vertex() : pos(0,0), visited(false) {

}

Vertex::Vertex(double x, double y) : pos(x, y), visited(false) {

}

Vertex::~Vertex() {
}

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
}

Map::~Map() {
    // std::cout << "~Map" << std::endl;
    // for (Obstacle *obs : this->m_list_obstacle) {
    //     delete obs;
    // }
    // delete this->m_graph;
}

void Map::constructGraph() {
    VoronoiBuilder vb;
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
    vb.insert_segment(p0.x, p0.y, p1.x, p1.y);
    vb.insert_segment(p1.x, p1.y, p2.x, p2.y);
    vb.insert_segment(p2.x, p2.y, p3.x, p3.y);
    vb.insert_segment(p3.x, p3.y, p0.x, p0.y);
    listSegment.push_back(Segment(Point(p0.x, p0.y),Point(p1.x,p1.y)));
    listSegment.push_back(Segment(Point(p1.x, p1.y),Point(p2.x,p2.y)));
    listSegment.push_back(Segment(Point(p2.x, p2.y),Point(p3.x,p3.y)));
    listSegment.push_back(Segment(Point(p3.x, p3.y),Point(p0.x,p0.y)));
    for (Obstacle *obs : this->m_listObstacle) {
        std::vector<Point>::iterator it0 = std::prev(obs->listPoint.end());
        std::vector<Point>::iterator it1 = obs->listPoint.begin();
        for (; it1 != obs->listPoint.end(); it0 = it1, it1++) {
            listPoint.push_back(*it1);
            vb.insert_segment(it0->x, it0->y, it1->x, it1->y);
            listSegment.push_back(Segment(Point(it0->x, it0->y),Point(it1->x,it1->y)));
        }
    }
    vb.construct(&vd);
    std::vector<VoronoiVertex> &list_vertex = const_cast<std::vector<VoronoiVertex>&>(vd.vertices());
    int32_t idx = 0;
    for (VoronoiVertex &v : list_vertex) {
        this->m_list_vertex.push_back(new Vertex(v.x(), v.y()));
        v.index = idx;
        idx++;
    }
    std::vector<VoronoiEdge> &list_edge = const_cast<std::vector<VoronoiEdge>&>(vd.edges());
    for (const Obstacle *obs : this->m_listObstacle) {
        for (VoronoiEdge &edge : list_edge) {
            if (edge.is_primary() && edge.is_finite()) {
                const VoronoiVertex *v0 = edge.vertex0();
                const VoronoiVertex *v1 = edge.vertex1();
                if (obs->contains(v0->x(), v0->y()) && obs->contains(v1->x(), v1->y())) {
                    edge.twin()->set_secondary();
                    edge.set_secondary();
                }
                if (edge.is_primary()) {
                    Vertex *vertex0 = this->m_list_vertex[v0->index];
                    Vertex *vertex1 = this->m_list_vertex[v1->index];
                    vertex0->neighbors.push_back(vertex1);
                    vertex1->neighbors.push_back(vertex0);
                }
            }
        }
    }
    for (VoronoiEdge &edge : list_edge) {
        if (edge.is_finite() && edge.is_primary()) {
            auto cell = edge.cell();
            auto v0 = edge.vertex0();
            auto v1 = edge.vertex1();
            auto seg = listSegment[cell->source_index()];
            Vertex *vertex0 = this->m_list_vertex[v0->index];
            Vertex *vertex1 = this->m_list_vertex[v1->index];

            Vector2d p1p0(seg.p1.x - seg.p0.x, seg.p1.y - seg.p0.y);
            Vector2d v0p0(v0->x() - seg.p0.x, v0->y() - seg.p0.y);
            Vector2d v1p0(v1->x() - seg.p0.x, v1->y() - seg.p0.y);
            double lambda0 = dot_product(v0p0,p1p0) / dot_product(p1p0,p1p0);
            double lambda1 = dot_product(v1p0,p1p0) / dot_product(p1p0,p1p0);
            if (vertex0->obsPoints.size() < vertex0->neighbors.size()) {
                if (lambda0 <= 0) {
                    vertex0->addObsPoint(seg.p0);
                } else if (lambda0 >= 1) {
                    vertex0->addObsPoint(seg.p1);
                } else {
                    Vector2d s0 = Vector2d(seg.p0.x, seg.p0.y) + lambda0*p1p0;
                    vertex0->addObsPoint(Point(s0.x, s0.y));
                }
            }
            if (vertex1->obsPoints.size() < vertex1->neighbors.size()) {
                if (lambda1 <= 0) {
                    vertex1->addObsPoint(seg.p0);
                } else if (lambda1 >= 1) {
                    vertex1->addObsPoint(seg.p1);
                } else {
                    Vector2d s1 = Vector2d(seg.p0.x, seg.p0.y) + lambda1*p1p0;
                    vertex1->addObsPoint(Point(s1.x, s1.y));
                }
            }
        }
    }
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

std::vector<Obstacle*> Map::getListObstacle() const {
    return m_listObstacle;
}

std::vector<Vertex*> Map::getListVertex() const {
    return m_list_vertex;
}

int32_t Map::getWidth() const {
    return m_width;
}

int32_t Map::getHeight() const {
    return m_height;
}

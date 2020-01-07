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
#include <unordered_map>
#include <fstream>
#include <iostream>

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
}

Map::~Map() {
    // std::cout << "~Map" << std::endl;
    // for (Obstacle *obs : this->m_list_obstacle) {
    //     delete obs;
    // }
    // delete this->m_graph;
}

void Map::saveTofile(const std::string &filename) {
    std::ofstream outFile(filename);
    outFile << this->m_width << ' ' << this->m_height << std::endl;
    for (Obstacle *obs : this->m_listObstacle) {
        for (Point &p : obs->listPoint) {
            outFile << '(' << int(p.x) << ',' << int(p.y) << ") ";
        }
        outFile << std::endl;
    }
    outFile.close();
}

std::vector<std::vector<Point>> Map::readFromFile(const std::string &filename) {
    std::ifstream inFile(filename);
    int32_t width;
    int32_t height;
    inFile >> width >> height;
    std::vector<Point> listPoint;
    std::vector<std::vector<Point>> listObs;
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
            listObs.push_back(listPoint);
            listPoint.clear();
        }
    }
    return listObs;
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

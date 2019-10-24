//
//  Map.cpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#include "Map.hpp"
#include <iostream>
#include <fstream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>

Obstacle::Obstacle(std::vector<Point> t_listPoint) : listPoint(t_listPoint) {
};

void Obstacle::move(int32_t t_deltaX, int32_t t_deltaY) {
    for (Point &point : this->listPoint) {
        point.x += t_deltaX;
        point.y += t_deltaY;
    }
}

bool Obstacle::contains(int32_t t_posX, int32_t t_posY) {
    typedef boost::geometry::model::d2::point_xy<double> point_type;
    typedef boost::geometry::model::polygon<point_type> Polygon;
    Polygon poly;
    for (const Point &p : this->listPoint) {
        boost::geometry::append(poly.outer(), point_type(p.x, p.y));
    }
    Point &pFirst = this->listPoint[0];
    boost::geometry::append(poly.outer(), point_type(pFirst.x, pFirst.y));
    return boost::geometry::within(point_type(t_posX, t_posY), poly);
}

void Obstacle::print() {
    std::cout << "============" << std::endl;
    for (Point &point : this->listPoint) {
        std::cout << point.x << ',' << point.y << std::endl;
    }
    std::cout << "============" << std::endl;
}

Map::Map(int32_t t_width, int32_t t_height) : m_width(t_width), m_height(t_height) {
    this->m_vd = NULL;
}

Map::~Map() {
    // std::cout << "~Map" << std::endl;
    // for (Obstacle *obs : this->m_list_obstacle) {
    //     delete obs;
    // }
    // delete this->m_graph;
}

void Map::constructVoronoi() {
    std::vector<Segment> listSegment;
    std::vector<Point> &listPoint = this->m_listVDPoint;
    listPoint.clear();
    Point p0 = Point(0, 0);
    Point p1 = Point(0, this->m_height);
    Point p2 = Point(this->m_width, this->m_height);
    Point p3 = Point(this->m_width, 0);
    listPoint.push_back(p0);
    listPoint.push_back(p1);
    listPoint.push_back(p2);
    listPoint.push_back(p3);
    listSegment.push_back(Segment(p0, p1));
    listSegment.push_back(Segment(p1, p2));
    listSegment.push_back(Segment(p2, p3));
    listSegment.push_back(Segment(p3, p0));
    if (!this->m_vd) {
        this->m_vd = new VoronoiDiagram;
    }
    this->m_vd->clear();
    for (Obstacle *obs : this->m_listObstacle) {
        std::vector<Point>::iterator it0 = std::prev(obs->listPoint.end());
        std::vector<Point>::iterator it1 = obs->listPoint.begin();
        for (;it1 != obs->listPoint.end();it0 = it1, it1++) {
            listPoint.push_back(*it1);
            listSegment.push_back(Segment(*it0,*it1));
        }
    }
    boost::polygon::construct_voronoi(listSegment.begin(), listSegment.end(), this->m_vd);
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
                    std::cout << x << ',' << y << std::endl;
                    listPoint.push_back(Point(x,y));
                } else {
                    std::cout << "faild" << std::endl;
                    throw;
                }
            }
            Obstacle *obs = map.addObstacle(std::vector<Point>(listPoint));
            listPoint.clear();
        }
    }
    return map;
}

Obstacle* Map::addObstacle(const std::vector<Point>& t_listPoint) {
    Obstacle *obs = new Obstacle(t_listPoint);
    this->m_listObstacle.push_back(obs);
    return obs;
}

std::vector<Obstacle*> Map::getListObstacle() const {
    return m_listObstacle;
}

VoronoiDiagram* Map::getVoronoiDiagram() const {
    return this->m_vd;
}

std::vector<Point> Map::getListVDPoint() const {
    return this->m_listVDPoint;
}

int32_t Map::getWidth() const {
    return m_width;
}

int32_t Map::getHeight() const {
    return m_height;
}

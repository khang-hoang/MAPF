//
//  Map.hpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#ifndef Map_hpp
#define Map_hpp

#include <stdio.h>
#include <vector>
#include "TypeDefine.hpp"

class Obstacle {
public:
    std::vector<Point> listPoint;
    Obstacle(std::vector<Point> t_listPoint);
    void move(int32_t t_deltaX, int32_t t_deltaY);
    bool contains(int32_t t_posX, int32_t t_posY) const;
};

class Vertex {
public:
    Vertex();
    Vertex(double x, double y);
    ~Vertex();
    double x() const;
    double y() const;
    void setPos(double x, double y);
    void addObsPoint(const Point& p);
    std::vector<Vertex*> neighbors;
    std::vector<Point> obsPoints;
    Point pos;
    bool visited;
};

class Map {
private:
    int32_t m_width;
    int32_t m_height;
    std::vector<Obstacle*> m_listObstacle;
    std::vector<Vertex*> m_list_vertex;
public:
    Map(int32_t t_width, int32_t t_height);
    ~Map();
    // void add_obstacle(const Obstacle & t_obstacle);
    Obstacle* addObstacle(const std::vector<Point>& t_obstacle);
    std::vector<Obstacle*> getListObstacle() const;
    void constructGraph();
    std::vector<Vertex*> getGraph() const;
    std::vector<Vertex*> cloneGraph() const;
    int32_t getWidth() const;
    int32_t getHeight() const;
    void saveTofile(const std::string &filename);
    static Map readFromFile(const std::string &filename);
};

#endif /* Map_hpp */

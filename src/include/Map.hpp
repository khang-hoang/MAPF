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
#include <boost/graph/adjacency_list.hpp>

struct Point {
    int32_t x;
    int32_t y;
};

class Obstacle;

struct VertexProperties {
    Point pos;
    Obstacle *obs;
};

struct EdgeProperties { int blah; };

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties> Graph;
//Some typedefs for simplicity
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

class Obstacle {
public:
    std::vector<Vertex> list_vertex;
    Obstacle(std::vector<Vertex> t_list_vertex);
    void move(int32_t t_delta_x, int32_t t_delta_y);
};

class Map {
private:
    int32_t m_width;
    int32_t m_height;
    Graph m_graph;
    std::vector<Obstacle*> m_list_obstacle;
public:
    Map(int32_t t_width, int32_t t_height);
    // void add_obstacle(const Obstacle & t_obstacle);
    Obstacle* addObstacle(const std::vector<Point>& t_obstacle);
    std::vector<Obstacle*> getListObstacle();
    int32_t getWidth() const;
    int32_t getHeight() const;
};

#endif /* Map_hpp */

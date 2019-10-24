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
    void print();
    bool contains(int32_t t_posX, int32_t t_posY);
};

// struct VertexProperties {
//     Point pos;
// };

// struct EdgeProperties { 
// };

// typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties> Graph;
// //Some typedefs for simplicity
// typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
// typedef boost::graph_traits<Graph>::edge_descriptor Edge;

class Map {
private:
    int32_t m_width;
    int32_t m_height;
    std::vector<Obstacle*> m_listObstacle;
    std::vector<Point> m_listVDPoint;
    VoronoiDiagram *m_vd;
public:
    Map(int32_t t_width, int32_t t_height);
    ~Map();
    // void add_obstacle(const Obstacle & t_obstacle);
    Obstacle* addObstacle(const std::vector<Point>& t_obstacle);
    std::vector<Obstacle*> getListObstacle() const;
    VoronoiDiagram* getVoronoiDiagram() const;
    std::vector<Point> getListVDPoint() const;
    int32_t getWidth() const;
    int32_t getHeight() const;
    void constructVoronoi();
    void saveTofile(const std::string &filename);
    static Map readFromFile(const std::string &filename);
};

#endif /* Map_hpp */

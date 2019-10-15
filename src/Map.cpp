//
//  Map.cpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#include "Map.hpp"

Obstacle::Obstacle(std::vector<Vertex> t_list_vertex) : list_vertex(t_list_vertex) {

}

Map::Map(int32_t t_width, int32_t t_height) : m_width(t_width), m_height(t_height) {

}

Obstacle* Map::addObstacle(const std::vector<Point>& t_obstacle) {
    std::vector<Vertex> list_vertex;
    for (const Point &p : t_obstacle) {
        Vertex v = boost::add_vertex(m_graph);
        m_graph[v].pos = p;
        list_vertex.push_back(v);
    }
    Obstacle *obs = new Obstacle(list_vertex);
    for (const Vertex &v : list_vertex) {
        m_graph[v].obs = obs;
    }
    m_list_obstacle.push_back(obs);
    return obs;
}

std::vector<Obstacle*> Map::getListObstacle() {
    return m_list_obstacle;
}

int32_t Map::getWidth() const {
    return m_width;
}

int32_t Map::getHeight() const {
    return m_height;
}

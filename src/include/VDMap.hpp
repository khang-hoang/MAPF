//
//  VDMap.hpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#ifndef VDMap_hpp
#define VDMap_hpp

#include "Map.hpp"
#include <stdio.h>
#include <vector>
#include "TypeDefine.hpp"

class VDMap : public Map {
typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point;
typedef boost::geometry::model::box<point> box;
typedef std::pair<box, std::vector<Vertex*>> value;
typedef boost::geometry::index::rtree<value, boost::geometry::index::rstar<4>> rtree;
private:
    rtree *m_rtree;
public:
    VDMap(int32_t t_width, int32_t t_height);
    ~VDMap();
    void constructGraph();
    std::vector<Vertex*> getNearestEdge(const Point& pos);
};

#endif /* VDMap_hpp */

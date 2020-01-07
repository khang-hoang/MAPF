//
//  DTMap.hpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#ifndef DTMap_hpp
#define DTMap_hpp

#include "Map.hpp"
#include <stdio.h>
#include <vector>
#include "TypeDefine.hpp"

class DTMap : public Map {
private:
public:
    DTMap(int32_t t_width, int32_t t_height);
    ~DTMap();
    void constructGraph();
    std::vector<Obstacle*> getEnlargedObs(double offset);
};

#endif /* DTMap_hpp */

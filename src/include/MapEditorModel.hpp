#ifndef __MAPEDITORMODEL_HPP__
#define __MAPEDITORMODEL_HPP__

#include "Map.hpp"
#include "EditMode.hpp"

struct MapEditorModel {
    sf::Font font;
    EditMode editMode;
    int viewX;
    int viewY;
    double zoomLevel;
    Point *selectedVertex;
    Obstacle *selectedObstacle;
};

#endif // __MAPEDITORMODEL_HPP__

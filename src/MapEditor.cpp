#include "MapEditor.hpp"
#include <iostream>
#include <fstream>

MapEditor::MapEditor(Map &t_map, MapEditorModel &t_model, MapView &t_mapView) : m_editorModel(t_model),  m_map(t_map), m_mapView(t_mapView) {
}

MapEditor::~MapEditor() {
}

bool circleContain(int32_t centerX, int32_t centerY, int32_t radius, int32_t posX, int32_t posY) {
    return sqrt(pow(centerX - posX, 2) + pow(centerY - posY, 2)) < radius;
}

double distance(const Point &p0, const Point &p1) {
    return hypot(p1.x - p0.x, p1.y - p0.y);
}

void MapEditor::handleEvent(const sf::RenderWindow &t_window, sf::Event &t_event) {
    if (t_event.type == sf::Event::MouseButtonPressed) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f pos = this->m_mapView.getMousePosition(t_window);
            std::cout << pos.x << ',' << pos.y << std::endl;
            this->m_oldMousePos = pos;
            if (this->m_editorModel.selectedObstacle) {
                std::vector<Point*> clickedPoints;
                for (Obstacle *obs : this->m_map.getListObstacle()) {
                    for (Point &vertex : obs->listPoint) {
                        if (circleContain(vertex.x, vertex.y, 5, pos.x, pos.y)) {
                            clickedPoints.push_back(&vertex);
                        }
                    }
                }
                if (clickedPoints.size() > 0) {
                    this->m_editorModel.selectedVertex = clickedPoints[0];
                }
            }
            // if (!this->m_editorModel.selectedObstacle) {
            std::vector<Obstacle*> clickedObs;
            for (Obstacle *obs : this->m_map.getListObstacle()) {
                if (obs->contains(pos.x, pos.y)) {
                    clickedObs.push_back(obs);
                }
            }
            Obstacle *tmpSelected = NULL;
            if (clickedObs.size() > 0) {
                tmpSelected = clickedObs[0];
            }
            if (!tmpSelected && this->m_editorModel.selectedObstacle && !this->m_editorModel.selectedVertex) {
                std::vector<Point> &listPoint = this->m_editorModel.selectedObstacle->listPoint;
                std::vector<Point>::iterator p0 = listPoint.end() - 1;
                std::vector<Point>::iterator p1 = listPoint.begin();
                std::vector<Point>::iterator it = p1;
                Point mousePos(pos.x, pos.y);
                double minDistance = distance(*p0, mousePos) + distance(*p1, mousePos);
                p0 = p1++;
                for (;p1 != listPoint.end(); p0 = p1, p1++) {
                    double d = distance(*p0, mousePos) + distance(*p1, mousePos);
                    if (d < minDistance) {
                        minDistance = d;
                        it = p1;
                    }
                }
                listPoint.insert(it, mousePos);
            } else if (tmpSelected && tmpSelected != this->m_editorModel.selectedObstacle) {
                this->m_editorModel.selectedObstacle = tmpSelected;
            }
        }
    }
    if (t_event.type == sf::Event::MouseMoved) {
        sf::Vector2f pos = this->m_mapView.getMousePosition(t_window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (this->m_editorModel.selectedVertex) {
                this->m_editorModel.selectedVertex->x = pos.x;
                this->m_editorModel.selectedVertex->y = pos.y;
            } else if (this->m_editorModel.selectedObstacle) {
                this->m_editorModel.selectedObstacle->move(pos.x - this->m_oldMousePos.x, pos.y - this->m_oldMousePos.y);
            }
        }
        this->m_oldMousePos = pos;
    }
    if (t_event.type == sf::Event::MouseButtonReleased) {
        this->m_editorModel.selectedVertex = NULL;
    }
    if (t_event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            this->m_editorModel.selectedObstacle = NULL;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
            this->m_editorModel.editMode = EditMode::EditVertex;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            this->m_editorModel.editMode = EditMode::EditEdge;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
            this->m_editorModel.editMode = EditMode::EditObstacle;
        }
    }
}
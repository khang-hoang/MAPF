#include "MapEditor.hpp"
#include <fstream>
#include <iostream>

MapEditor::MapEditor(Map &t_map, MapEditorModel &t_model, MapView &t_mapView) : m_editorModel(t_model), m_map(t_map), m_mapView(t_mapView), m_isCtrlDown(false), m_isShiftDown(false) {
}

MapEditor::~MapEditor() {
}

bool circleContain(int32_t centerX, int32_t centerY, int32_t radius, int32_t posX, int32_t posY) {
    return sqrt(pow(centerX - posX, 2) + pow(centerY - posY, 2)) < radius;
}

double distance(const Point &p0, const Point &p1) {
    return hypot(p1.x - p0.x, p1.y - p0.y);
}

sf::Vector2f MapEditor::getMousePos() const {
    return this->m_oldMousePos;
}

void MapEditor::handleEvent(const sf::RenderWindow &t_window, sf::Event &t_event) {
    double zoomLevel = this->m_editorModel.zoomLevel;
    if (t_event.type == sf::Event::KeyPressed) {
        if (t_event.key.code == sf::Keyboard::LControl) {
            this->m_isCtrlDown = true;
        }
        if (t_event.key.code == sf::Keyboard::LShift) {
            this->m_isShiftDown = true;
        }
        if (t_event.key.code == sf::Keyboard::Escape) {
            if (this->m_editorModel.selectedObstacle || this->m_editorModel.selectedVertex) {
                this->m_editorModel.selectedObstacle = NULL;
                this->m_editorModel.selectedVertex = NULL;
                t_event.key.code = sf::Keyboard::Unknown;
            }
        }
        if (t_event.key.code == sf::Keyboard::Delete) {
            if (this->m_editorModel.selectedObstacle) {
                this->m_map.removeObstacle(this->m_editorModel.selectedObstacle);
                this->m_map.constructGraph();
                this->m_editorModel.selectedObstacle = NULL;
            }
        }
    }
    if (t_event.type == sf::Event::KeyReleased) {
        if (t_event.key.code == sf::Keyboard::LControl) {
            this->m_isCtrlDown = false;
        }
        if (t_event.key.code == sf::Keyboard::LShift) {
            this->m_isShiftDown = false;
        }
    }
    if (t_event.type == sf::Event::MouseButtonPressed) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f pos = this->m_mapView.getMousePosition(t_window);
            sf::Vector2f viewPos = sf::Vector2f(this->m_editorModel.viewX, this->m_editorModel.viewY);
            sf::Vector2f zoomPos = sf::Vector2f((pos.x+viewPos.x)/zoomLevel, (pos.y+viewPos.y)/zoomLevel);
            this->m_oldMousePos = pos;
            if (this->m_editorModel.selectedObstacle) {
                std::vector<Point*> clickedPoints;
                for (Obstacle *obs : this->m_map.getListObstacle()) {
                    for (Point &vertex : obs->listPoint) {
                        if (circleContain(vertex.x, vertex.y, 5, zoomPos.x, zoomPos.y)) {
                            clickedPoints.push_back(&vertex);
                        }
                    }
                }
                if (clickedPoints.size() > 0) {
                    this->m_editorModel.selectedVertex = clickedPoints[0];
                }
                if (this->m_isShiftDown && this->m_editorModel.selectedObstacle && this->m_editorModel.selectedVertex) {
                    this->m_editorModel.selectedObstacle->removePoint(*this->m_editorModel.selectedVertex);
                    this->m_map.constructGraph();
                    this->m_editorModel.selectedVertex = NULL;
                }
            }
            std::vector<Obstacle *> clickedObs;
            for (Obstacle *obs : this->m_map.getListObstacle()) {
                if (obs->contains(zoomPos.x, zoomPos.y)) {
                    clickedObs.push_back(obs);
                }
            }
            Obstacle *tmpSelected = NULL;
            if (clickedObs.size() > 0) {
                tmpSelected = clickedObs[0];
            }
            if (!this->m_isCtrlDown && !this->m_isShiftDown) {
                if (!tmpSelected) {
                    if (!this->m_editorModel.selectedObstacle) {
                        std::vector<Point> obs;
                        obs.push_back(Point(zoomPos.x-20, zoomPos.y-20));
                        obs.push_back(Point(zoomPos.x+20, zoomPos.y-20));
                        obs.push_back(Point(zoomPos.x+20, zoomPos.y+20));
                        obs.push_back(Point(zoomPos.x-20, zoomPos.y+20));
                        this->m_map.addObstacle(obs);
                        this->m_map.constructGraph();
                    } else if (!this->m_editorModel.selectedVertex) {
                        std::vector<Point> &listPoint = this->m_editorModel.selectedObstacle->listPoint;
                        std::vector<Point>::iterator p0 = listPoint.end() - 1;
                        std::vector<Point>::iterator p1 = listPoint.begin();
                        std::vector<Point>::iterator it = p1;
                        Point mousePos(zoomPos.x, zoomPos.y);
                        double minDistance = distance(*p0, mousePos) + distance(*p1, mousePos);
                        p0 = p1++;
                        for (; p1 != listPoint.end(); p0 = p1, p1++) {
                            double d = distance(*p0, mousePos) + distance(*p1, mousePos);
                            if (d < minDistance) {
                                minDistance = d;
                                it = p1;
                            }
                        }
                        listPoint.insert(it, mousePos);
                        this->m_map.constructGraph();
                    }
                } else if (tmpSelected != this->m_editorModel.selectedObstacle) {
                   this->m_editorModel.selectedObstacle = tmpSelected;
                }
            }
        }
    }
    if (t_event.type == sf::Event::MouseMoved) {
        sf::Vector2f pos = this->m_mapView.getMousePosition(t_window);
        sf::Vector2f viewPos = sf::Vector2f(this->m_editorModel.viewX, this->m_editorModel.viewY);
        sf::Vector2f zoomPos = sf::Vector2f((pos.x+viewPos.x)/zoomLevel, (pos.y+viewPos.y)/zoomLevel);
        sf::Vector2f oldZoomPos((this->m_oldMousePos.x+viewPos.x)/zoomLevel, (this->m_oldMousePos.y+viewPos.y)/zoomLevel);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (this->m_isCtrlDown) {
                this->m_editorModel.viewX -= (pos.x - this->m_oldMousePos.x);
                this->m_editorModel.viewY -= (pos.y - this->m_oldMousePos.y);
            } else if (!this->m_isShiftDown && this->m_editorModel.selectedVertex) {
                this->m_editorModel.selectedVertex->x = zoomPos.x;
                this->m_editorModel.selectedVertex->y = zoomPos.y;
                this->m_map.constructGraph();
            } else if (this->m_editorModel.selectedObstacle) {
                this->m_editorModel.selectedObstacle->move(zoomPos.x - oldZoomPos.x, zoomPos.y - oldZoomPos.y);
                this->m_map.constructGraph();
            }

        }
        this->m_oldMousePos = pos;
    }
    if (t_event.type == sf::Event::MouseButtonReleased) {
        this->m_editorModel.selectedVertex = NULL;
    }
    if (this->m_isCtrlDown && t_event.type == sf::Event::MouseWheelMoved) {
        this->m_editorModel.zoomLevel += t_event.mouseWheel.delta*0.1;
        if (abs(this->m_editorModel.zoomLevel - 1.0) < 0.0001) {
            this->m_editorModel.zoomLevel = 1.0;
        }
        if (this->m_editorModel.zoomLevel < 0.1) {
            this->m_editorModel.zoomLevel = 0.1;
        }
        if (this->m_editorModel.zoomLevel > 3.0) {
            this->m_editorModel.zoomLevel = 3.0;
        }
    }
}

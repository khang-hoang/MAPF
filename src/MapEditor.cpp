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

void MapEditor::handleEvent(const sf::RenderWindow &t_window, sf::Event t_event) {
    if (t_event.type == sf::Event::MouseButtonPressed) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f pos = this->m_mapView.getMousePosition(t_window);
            this->m_oldMousePos = pos;
            if (this->m_editorModel.editMode == EditMode::EditVertex && !this->m_editorModel.selectedVertex) {
                std::vector<Point*> clickedPoints;
                for (Obstacle *obs : this->m_map.getListObstacle()) {
                    for (Point &vertex : obs->listPoint) {
                        if (circleContain(vertex.x, vertex.y, 3, pos.x, pos.y)) {
                            clickedPoints.push_back(&vertex);
                        }
                    }
                }
                if (clickedPoints.size() > 0) {
                    this->m_editorModel.selectedVertex = clickedPoints[0];
                }
            }
            if (this->m_editorModel.editMode == EditMode::EditObstacle && !this->m_editorModel.selectedObstacle) {
                std::vector<Obstacle*> clickedObs;
                for (Obstacle *obs : this->m_map.getListObstacle()) {
                   if (obs->contains(pos.x, pos.y)) {
                       clickedObs.push_back(obs);
                   }
                }
                if (clickedObs.size() > 0) {
                    this->m_editorModel.selectedObstacle = clickedObs[0];
                }
            }
        }
    }
    if (t_event.type == sf::Event::MouseMoved) {
        sf::Vector2f pos = this->m_mapView.getMousePosition(t_window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (this->m_editorModel.editMode == EditMode::EditVertex && this->m_editorModel.selectedVertex) {
                this->m_editorModel.selectedVertex->x = pos.x;
                this->m_editorModel.selectedVertex->y = pos.y;
            }
            if (this->m_editorModel.editMode == EditMode::EditObstacle && this->m_editorModel.selectedObstacle) {
                this->m_editorModel.selectedObstacle->move(pos.x - this->m_oldMousePos.x, pos.y - this->m_oldMousePos.y);
            }
        }
        this->m_oldMousePos = pos;
    }
    if (t_event.type == sf::Event::MouseButtonReleased) {
        this->m_editorModel.selectedVertex = NULL;
        this->m_editorModel.selectedObstacle= NULL;
    }
    if (t_event.type == sf::Event::KeyPressed) {
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
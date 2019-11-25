#include "MapView.hpp"
#include <array>
#include "Color.hpp"
#include "LineShape.hpp"
#include "PolygonShape.hpp"
#include "TypeDefine.hpp"
#include "config.hpp"

MapView::MapView(const Map &t_map, MapEditorModel &t_model) : m_editorModel(t_model), m_map(t_map) {
}

sf::Vector2f MapView::getMousePosition(const sf::RenderWindow &t_window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(t_window);
    sf::Vector2f currentPos = this->getPosition();
    return sf::Vector2f(mousePos.x - currentPos.x, mousePos.y - currentPos.y);
}

void MapView::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();
    // draw boundingbox
    sf::RectangleShape rect(sf::Vector2f(this->m_map.getWidth(),this->m_map.getHeight()));
    rect.setOutlineColor(Color::Black);
    rect.setOutlineThickness(1);
    target.draw(rect, states);
    // draw obstacle
    std::vector<sf::CircleShape> listPoint;
    for (Obstacle *obs : this->m_map.getListObstacle()) {
        PolygonShape polygon(obs->listPoint);
        if (obs == this->m_editorModel.selectedObstacle) {
            for (Point &vertex : obs->listPoint) {
                sf::CircleShape point;
                point.setRadius(3);
                point.setFillColor(Color::Alizarin);
                point.setPosition(sf::Vector2f(vertex.x, vertex.y));
                point.setOrigin(3, 3);
                listPoint.push_back(point);
            }
        }
        polygon.setFillColor(Color::Asbestos);
        target.draw(polygon,states);
    }
    // draw graph
    std::vector<Vertex*> graph = this->m_map.cloneGraph();
    for (Vertex *v0 : graph) {
        for (Point p : v0->obsPoints) {
            sf::LineShape line(sf::Vector2f(v0->x(), v0->y()), sf::Vector2f(p.x,p.y));
            line.setFillColor(Color::Orange);
            line.setThickness(1);
            target.draw(line, states);
        }
        for (Vertex *v1 : v0->neighbors) {
            if (!v1->visited) {
                sf::LineShape line(sf::Vector2f(v0->x(), v0->y()), sf::Vector2f(v1->x(),v1->y()));
                line.setFillColor(Color::BelizeHole);
                line.setThickness(1.5);
                target.draw(line, states);
            }
        }
        if (v0->obsPoints.size() > 2) {
            sf::CircleShape point;
            point.setPosition(sf::Vector2f(v0->x(), v0->y()));
            point.setRadius(3);
            point.setFillColor(Color::MidnightBlue);
            point.setOrigin(3, 3);
            listPoint.push_back(point);
        } else if (v0->obsPoints.size() == 2) {
            sf::CircleShape point;
            point.setPosition(sf::Vector2f(v0->x(), v0->y()));
            point.setRadius(2);
            point.setFillColor(Color::MidnightBlue);
            point.setOrigin(2, 2);
            listPoint.push_back(point);
        }
        v0->visited = true;
    }
    for (const sf::CircleShape &point : listPoint) {
        target.draw(point, states);
    }
    for (Vertex *v : graph) {
        delete v;
    }
}

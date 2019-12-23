#include "MapView.hpp"
#include <array>
#include "Color.hpp"
#include "LineShape.hpp"
#include "PolygonShape.hpp"
#include "TypeDefine.hpp"
#include "config.hpp"

MapView::MapView(const Map &t_map, MapEditorModel &t_model) : m_editorModel(t_model), m_map(t_map) {
    this->m_renderer.create(this->m_map.getWidth()*3.1,this->m_map.getHeight()*3.1, sf::ContextSettings(0, 0, 8));
}

sf::Vector2f MapView::getMousePosition(const sf::RenderWindow &t_window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(t_window);
    sf::Vector2f currentPos = this->getPosition();
    return sf::Vector2f(mousePos.x - currentPos.x, mousePos.y - currentPos.y);
}

void MapView::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    this->m_renderer.clear(sf::Color::White);
    sf::RenderStates rendererStates = sf::RenderStates::Default;
    rendererStates.transform.translate(2,2);
    double zoomLevel = this->m_editorModel.zoomLevel;
    // draw boundingbox
    sf::RectangleShape mapRect(sf::Vector2f(this->m_map.getWidth()*zoomLevel,this->m_map.getHeight()*zoomLevel));
    mapRect.setOutlineThickness(1);
    mapRect.setOutlineColor(Color::Black);
    this->m_renderer.draw(mapRect, rendererStates);

    // draw obstacle
    std::vector<sf::CircleShape> listPoint;
    for (Obstacle *obs : this->m_map.getListObstacle()) {
        std::vector<Point> obsPoints;
        for (Point &p : obs->listPoint) {
            obsPoints.push_back(Point(p.x*zoomLevel, p.y*zoomLevel));
        }
        PolygonShape polygon(obsPoints);
        if (obs == this->m_editorModel.selectedObstacle) {
            for (Point &vertex : obs->listPoint) {
                sf::CircleShape point;
                point.setRadius(3);
                point.setFillColor(Color::Alizarin);
                point.setPosition(sf::Vector2f(vertex.x*zoomLevel, vertex.y*zoomLevel));
                point.setOrigin(3, 3);
                listPoint.push_back(point);
            }
        }
        polygon.setFillColor(Color::Asbestos);
        this->m_renderer.draw(polygon, rendererStates);
    }
    // draw graph
    std::vector<Vertex*> graph = this->m_map.cloneGraph();
    for (Vertex *v0 : graph) {
        for (Point p : v0->obsPoints) {
            sf::LineShape line(sf::Vector2f(v0->x()*zoomLevel, v0->y()*zoomLevel), sf::Vector2f(p.x*zoomLevel,p.y*zoomLevel));
            line.setFillColor(Color::Orange);
            line.setThickness(1);
            this->m_renderer.draw(line, rendererStates);
        }
        for (Vertex *v1 : v0->neighbors) {
            if (!v1->visited) {
                sf::LineShape line(sf::Vector2f(v0->x()*zoomLevel, v0->y()*zoomLevel), sf::Vector2f(v1->x()*zoomLevel,v1->y()*zoomLevel));
                line.setFillColor(Color::BelizeHole);
                line.setThickness(1.5);
                this->m_renderer.draw(line, rendererStates);
            }
        }
        if (v0->obsPoints.size() > 2) {
            sf::CircleShape point;
            point.setPosition(sf::Vector2f(v0->x()*zoomLevel, v0->y()*zoomLevel));
            point.setRadius(3);
            point.setFillColor(Color::MidnightBlue);
            point.setOrigin(3, 3);
            listPoint.push_back(point);
        } else if (v0->obsPoints.size() == 2) {
            sf::CircleShape point;
            point.setPosition(sf::Vector2f(v0->x()*zoomLevel, v0->y()*zoomLevel));
            point.setRadius(2);
            point.setFillColor(Color::MidnightBlue);
            point.setOrigin(2, 2);
            listPoint.push_back(point);
        }
        v0->visited = true;
    }
    for (const sf::CircleShape &point : listPoint) {
        this->m_renderer.draw(point, rendererStates);
    }
    this->m_renderer.display();
    states.transform *= getTransform();

    sf::Vector2u rectSize = target.getSize() - sf::Vector2u(60,60);

    sf::RectangleShape rect(sf::Vector2f(rectSize.x,rectSize.y));
    rect.setTexture(&this->m_renderer.getTexture());
    rect.setFillColor(sf::Color::White);
    rect.setTextureRect(sf::IntRect(this->m_editorModel.viewX + 2, this->m_editorModel.viewY + 2, rectSize.x, rectSize.y));
    rect.setOutlineThickness(1);
    rect.setOutlineColor(Color::Black);
    target.draw(rect, states);
    for (Vertex *v : graph) {
        delete v;
    }
}

#include "MapView.hpp"
#include <array>
#include "Color.hpp"
#include "LineShape.hpp"
#include "PolygonShape.hpp"
#include "TypeDefine.hpp"
#include "config.hpp"

MapView::MapView(const Map &t_map, MapEditorModel &t_model) : m_editorModel(t_model), m_map(t_map) {
    const int32_t mapWidth = this->m_map.getWidth();
    const int32_t mapHeight = this->m_map.getHeight();
    this->m_renderer.create(mapWidth, mapHeight);
    this->m_view = sf::View(sf::FloatRect(0, 0, mapWidth, mapHeight));
    // double widthFactor = (double)config::MAPVIEW_WIDTH/this->m_map.getWidth();
    // double heightFactor = (double)config::MAPVIEW_HEIGHT/this->m_map.getHeight();
    this->m_view.setViewport(sf::FloatRect(0, 0, 1.f, 1.f));
}

void MapView::update() {
    this->m_renderer.setView(this->m_view);
    this->m_renderer.clear(Color::White);
    std::vector<sf::CircleShape> list_point;
    for (Obstacle *obs : this->m_map.getListObstacle()) {
        PolygonShape polygon(obs->listPoint);
        int i = 0;
        if (obs == this->m_editorModel.selectedObstacle) {
            for (Point &vertex : obs->listPoint) {
                sf::CircleShape point;
                point.setRadius(3);
                point.setFillColor(Color::Alizarin);
                point.setPosition(sf::Vector2f(vertex.x, vertex.y));
                point.setOrigin(3, 3);
                list_point.push_back(point);
            }
        }
        polygon.setFillColor(Color::PeterRiver);
        this->m_renderer.draw(polygon);
    }
    for (const sf::CircleShape &point : list_point) {
        this->m_renderer.draw(point);
    }
    for (Vertex *v0 : this->m_map.getListVertex()) {
        for (Vertex *v1 : v0->neighbors) {
            if (!v1->visited) {
                sf::LineShape line(sf::Vector2f(v0->x(), v0->y()), sf::Vector2f(v1->x(),v1->y()));
                line.setFillColor(Color::Asbestos);
                line.setThickness(1);
                this->m_renderer.draw(line);
            }
        }
        for (Point p : v0->obsPoints) {
            sf::LineShape line(sf::Vector2f(v0->x(), v0->y()), sf::Vector2f(p.x,p.y));
            line.setFillColor(Color::Green);
            line.setThickness(1);
            this->m_renderer.draw(line);
        }
        v0->visited = true;
    }
    for (Vertex *v : this->m_map.getListVertex()) {
        v->visited = false;
    }

    this->m_renderer.display();
}

sf::Vector2f MapView::getMousePosition(const sf::RenderWindow &t_window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(t_window);
    sf::Vector2f currentPos = this->getPosition();
    return this->m_renderer.mapPixelToCoords(sf::Vector2i(mousePos.x - currentPos.x, mousePos.y - currentPos.y), this->m_view);
}

void MapView::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::RectangleShape mapRect(this->m_view.getSize());
    mapRect.setTexture(&this->m_renderer.getTexture());
    mapRect.setTextureRect(sf::IntRect(0, 0, config::MAPVIEW_WIDTH, config::MAPVIEW_HEIGHT));
    mapRect.setOutlineThickness(1);
    mapRect.setOutlineColor(Color::Black);
    states.transform *= getTransform();
    target.draw(mapRect, states);
}

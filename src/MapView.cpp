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
    VoronoiDiagram *vd = this->m_map.getVoronoiDiagram();
    VoronoiDiagram::const_edge_iterator it = vd->edges().begin();
    for (; it != vd->edges().end(); it++) {
        if (!it->is_primary()) {
            continue;
        }
        if ((it->color() == 1)) {
            continue;
        }
        if (!it->is_finite()) {
            const VoronoiVertex *v0 = it->vertex0();
            // Again, only consider half the half-edges, ignore edge->vertex1()
            // to avoid overdrawing the lines
            if (v0) {
                // std::cout << "test" << std::endl;
                // std::cout << it->cell()->source_index() << std::endl;
                // Direction of infinite edge if perpendicular to direction
                // between the points owning the two half edges.
                // Take the rotated right vector and multiply by a large
                // enough number to reach your bounding box
                // const std::vector<Point> &listPoint = this->m_map.getListVDPoint();
                // Point p1 = listPoint[it->cell()->source_index()];
                // Point p2 = listPoint[it->twin()->cell()->source_index()];
                // int end_x = (p1.y - p2.y) * 5000;
                // int end_y = (p1.x - pa2.x) * -5000;
                // sf::LineShape line(sf::Vector2f(v0->x(), v0->y()), sf::Vector2f(end_x, end_y));
                // line.setFillColor(Color::Red);
                // line.setThickness(1);
                // this->m_renderer.draw(line);
            }
        } else {
            double x0 = it->vertex0()->x();
            double y0 = it->vertex0()->y();
            double x1 = it->vertex1()->x();
            double y1 = it->vertex1()->y();
            sf::LineShape line(sf::Vector2f(x0, y0), sf::Vector2f(x1, y1));
            line.setFillColor(Color::Asbestos);
            line.setThickness(1);
            this->m_renderer.draw(line);
        }
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

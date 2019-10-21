#include "MapView.hpp"
#include "Color.hpp"
#include "PolygonShape.hpp"
#include "TypeDefine.hpp"
#include "LineShape.hpp"
#include <array>

MapView::MapView(const Map &t_map, MapEditorModel &t_model) : m_editorModel(t_model), m_map(t_map) {
    this->m_renderer.create(this->m_map.getWidth(), this->m_map.getHeight());
    this->m_view = sf::View(sf::FloatRect(0.f, 0.f, this->m_map.getWidth(), this->m_map.getHeight()));
    this->m_view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
}

void MapView::update() {
    this->m_renderer.setView(this->m_view);
    this->m_renderer.clear(Color::White);
    std::vector<sf::CircleShape> list_point;
    for (Obstacle *obs : this->m_map.getListObstacle()) {
        PolygonShape polygon(obs->listPoint);
        int i = 0;
        for (Point &vertex : obs->listPoint) {
            sf::CircleShape point;
            point.setRadius(3);
            point.setFillColor(Color::Alizarin);
            point.setPosition(sf::Vector2f(vertex.x, vertex.y));
            point.setOrigin(3,3);
            list_point.push_back(point);
        }
        polygon.setFillColor(Color::BelizeHole);
        this->m_renderer.draw(polygon);
    }
    for (const sf::CircleShape &point : list_point) {
        this->m_renderer.draw(point);
    }
    VoronoiDiagram *vd = this->m_map.getVoronoiDiagram();
    VoronoiDiagram::const_edge_iterator it = vd->edges().begin();
    typedef VoronoiDiagram::vertex_type VoronoiVertex;
    for (;it != vd->edges().end(); it++) {
        if (!it->is_primary()) {
            continue;
        }
        if ((it->color() == 1)) {
            continue;
        }
        if (!it->is_finite()) {
            const VoronoiVertex* v0 = it->vertex0();
               // Again, only consider half the half-edges, ignore edge->vertex1()
               // to avoid overdrawing the lines
            if (v0) {
                // std::cout << "index" << std::endl;
                // std::cout << it->cell()->source_index() << std::endl;
                // Direction of infinite edge if perpendicular to direction
                // between the points owning the two half edges. 
                // Take the rotated right vector and multiply by a large 
                // enough number to reach your bounding box
                // point p1 = points[edge->cell()->source_index()];
                // point p2 = points[edge->twin()->cell()->source_index()];
                // int end_x = (p1.y() - p2.y()) * 640;
                // int end_y = (p1.x() - p2.x()) * -640;
                // draw_line(v0->x(), v0->y(),
                //         end_x, end_y);
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
    return sf::Vector2f(mousePos.x - currentPos.x, mousePos.y - currentPos.y);
}

void MapView::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    sf::RectangleShape mapRect(sf::Vector2f(500, 500));
    mapRect.setTexture(&this->m_renderer.getTexture());
    mapRect.setOutlineThickness(1);
    mapRect.setOutlineColor(Color::Black);
    states.transform *= getTransform();
    target.draw(mapRect, states);
}
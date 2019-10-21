#include "PolygonShape.hpp"
#include "Color.hpp"
#include "Color.hpp"
#include "earcut.hpp"
#include <array>
#include <iostream>

PolygonShape::PolygonShape(std::vector<Point> t_listPoint) : m_listPoint(t_listPoint) {

}

PolygonShape::~PolygonShape() {

}

void PolygonShape::setFillColor(sf::Color t_fillColor) {
    this->m_fillColor = t_fillColor;
}

void PolygonShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    using point_type = std::array<double, 2>;
    std::vector<std::vector<point_type>> polygon;
    std::vector<point_type> outer;
    for (const Point &p : this->m_listPoint) {
        point_type point = {p.x, p.y};
        outer.push_back(point);
    }
    polygon.push_back(outer);
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
    std ::vector<sf::ConvexShape> listTriangle;
    for (int i = 0; i < indices.size(); i+=3) {
        sf::ConvexShape triangle;
        triangle.setPointCount(3);
        triangle.setPoint(0, sf::Vector2f(this->m_listPoint[indices[i]].x, this->m_listPoint[indices[i]].y));
        triangle.setPoint(1, sf::Vector2f(this->m_listPoint[indices[i+1]].x, this->m_listPoint[indices[i+1]].y));
        triangle.setPoint(2, sf::Vector2f(this->m_listPoint[indices[i+2]].x, this->m_listPoint[indices[i+2]].y));
        triangle.setFillColor(this->m_fillColor);
        listTriangle.push_back(triangle);
    }
    states.transform *= this->getTransform();
    for (const sf::ConvexShape &shape : listTriangle) {
        target.draw(shape, states);
    }
}

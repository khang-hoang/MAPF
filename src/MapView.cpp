#include "MapView.hpp"
#include "Color.hpp"

MapView::MapView(Map t_map): m_map(t_map) {

}

void MapView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::RectangleShape boundingBox(sf::Vector2f(m_map.getWidth(), m_map.getHeight()));
    boundingBox.setOutlineColor(Color::Red);
    boundingBox.setOutlineThickness(1);
    states.transform *= getTransform();
    target.draw(boundingBox, states);
}
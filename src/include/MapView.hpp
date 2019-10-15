#ifndef __MAPVIEW_HPP__
#define __MAPVIEW_HPP__

#include <SFML/Graphics.hpp>
#include "Map.hpp"

class MapView : public sf::Drawable, public sf::Transformable {
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    Map m_map;
public:
    MapView(Map t_map);
};

#endif // __MAPVIEW_HPP__

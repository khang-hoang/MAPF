#ifndef __MAPVIEW_HPP__
#define __MAPVIEW_HPP__

#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "MapEditorModel.hpp"

class MapView : public sf::Drawable, public sf::Transformable {
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    const Map &m_map;
    sf::RenderTexture m_renderer;
    sf::View m_view;
    MapEditorModel &m_editorModel;
public:
    void update();
    MapView(const Map &t_map, MapEditorModel &t_model);
    sf::Vector2f getMousePosition(const sf::RenderWindow &t_window);
};

#endif // __MAPVIEW_HPP__

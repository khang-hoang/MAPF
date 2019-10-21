#ifndef __STATUSBAR_HPP__
#define __STATUSBAR_HPP__

#include <SFML/Graphics.hpp>
#include "EditMode.hpp"
#include "MapEditorModel.hpp"

class StatusBar : public sf::Drawable, public sf::Transformable {
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    MapEditorModel &m_editorModel;
    const int32_t height = 25;
    StatusBar(MapEditorModel &t_model);
    void setMode(EditMode t_mode);
    ~StatusBar();
};

#endif // __STATUSBAR_HPP__

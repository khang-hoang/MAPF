#ifndef __POLYGONSHAPE_HPP__
#define __POLYGONSHAPE_HPP__

#include <SFML/Graphics.hpp>
#include "TypeDefine.hpp"
#include <vector>

class PolygonShape : public sf::Drawable, public sf::Transformable {
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    std::vector<Point> m_listPoint;
    sf::Color m_fillColor;
public:
    PolygonShape(std::vector<Point> t_listPoint);
    void setFillColor(sf::Color t_fillColor);
    ~PolygonShape();
};

#endif // __POLYGONSHAPE_HPP__

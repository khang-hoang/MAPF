#include "StatusBar.hpp"
#include "config.hpp"
#include "Color.hpp"

void StatusBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::Vector2u size = target.getSize();
    sf::RectangleShape rect(sf::Vector2f(size.x, this->height));
    rect.setFillColor(Color::Silver);
    sf::Text vertexText("V", this->m_editorModel.font, 20);
    vertexText.setFillColor(Color::Concrete);
    vertexText.setPosition(15, 0);
    sf::Text edgeText("E", this->m_editorModel.font, 20);
    edgeText.setFillColor(Color::Concrete);
    edgeText.setPosition(45, 0);
    sf::Text obstacleText("O", this->m_editorModel.font, 20);
    obstacleText.setFillColor(Color::Concrete);
    obstacleText.setPosition(75, 0);
    if (this->m_editorModel.editMode == EditMode::EditVertex) {
        vertexText.setFillColor(Color::WetAsphalt);
    } else if (this->m_editorModel.editMode  == EditMode::EditEdge) {
        edgeText.setFillColor(Color::WetAsphalt);
    } else if (this->m_editorModel.editMode  == EditMode::EditObstacle) {
        obstacleText.setFillColor(Color::WetAsphalt);
    }
    states.transform *= this->getTransform();
    target.draw(rect, states);
    target.draw(vertexText, states);
    target.draw(edgeText, states);
    target.draw(obstacleText, states);
}

StatusBar::StatusBar(MapEditorModel &t_model) : m_editorModel(t_model) {
}

StatusBar::~StatusBar() {

}
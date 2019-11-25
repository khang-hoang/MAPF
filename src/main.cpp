#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "Color.hpp"
#include "Map.hpp"
#include "MapEditor.hpp"
#include "MapEditorModel.hpp"
#include "MapView.hpp"
#include "PolygonShape.hpp"
#include "ResourcePath.hpp"
#include "config.hpp"

const int32_t IgnoreEvent = 0xffffffff;

int main(int argc, char const **argv) {
    config::setExecutePath(argv[0]);
    std::string inFilename;
    int32_t MAP_WIDTH = 500;
    int32_t MAP_HEIGHT = 500;

    int32_t VIDEO_WIDTH = 1024;
    int32_t VIDEO_HEIGHT = 768;

    if (argc == 2) {
        inFilename = argv[1];
    } else if (argc >= 4) {
        VIDEO_WIDTH = std::stoi(argv[2]);
        VIDEO_HEIGHT = std::stoi(argv[3]);
    }

    sf::Font font;
    if (!font.loadFromFile(getResourcePath("Roboto-Regular.ttf"))) {
        return EXIT_FAILURE;
    }

    // Create the main window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(VIDEO_WIDTH, VIDEO_HEIGHT), "SFML window", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    // Set the Icon
    sf::Image icon;
    if (icon.loadFromFile(getResourcePath("icon.png"))) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    std::ifstream inFile(inFilename);
    Map map(MAP_WIDTH, MAP_HEIGHT);
    if (inFile.good()) {
        map = Map::readFromFile(inFilename);
    }
    // Map map(MAP_WIDTH, MAP_HEIGHT);
    // map.addObstacle({{50, 50}, {50, 200}, {200, 200}, {200,50}});
    // map.addObstacle({{250, 50}, {250, 200}, {400, 200}, {400,50}});
    // Obstacle *obs = map.addObstacle({{300, 450}, {400, 400}, {200, 250}, {250,300}});
    // obs->move(50,50);
    // obs->print();
    map.constructGraph();
    MapEditorModel editorModel = {
        font,
        EditMode::EditVertex};
    MapView mapView(map, editorModel);
    MapEditor mapEditor(map, editorModel, mapView);

    // PolygonShape poly({{300, 100}, {400, 400}, {200, 250}, {350,300}});
    // mapEditor.m_editorModel.test = 100;
    // std::cout << statusBar.m_editorModel.test << std::endl;
    // return 0;
    // MapRenderTexture mapRenderTexture(map);
    // sf::View mapView(sf::FloatRect(0.f, 0.f, 500, 500));
    // mapView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    sf::Clock clock;

    // Start the game loop
    while (window.isOpen()) {
        int fps = (int)(1.0 / clock.restart().asSeconds());
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            mapEditor.handleEvent(window, event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                VIDEO_WIDTH = event.size.width;
                VIDEO_HEIGHT = event.size.height;
                sf::FloatRect visibleArea(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
                window.setView(sf::View(visibleArea));
            }
        }

        // Clear screen
        window.clear(sf::Color::White);

        // Draw FPS
        sf::Text fpsText(std::to_string(fps), font, 16);
        fpsText.setFillColor(sf::Color::Black);
        window.draw(fpsText);

        sf::Vector2f mousePos = mapEditor.getMousePos();
        std::ostringstream ostr;
        ostr << int(mousePos.x) << ',' << int(mousePos.y);
        sf::Text mousePosText(ostr.str(), font, 16);
        mousePosText.setFillColor(sf::Color::Black);
        mousePosText.setPosition(30, 0);
        window.draw(mousePosText);

        // Draw map
        // map.constructGraph();
        mapView.setPosition(30, 30);
        window.draw(mapView);

        // Update the window
        window.display();
    }

    map.saveTofile(inFilename);

    return EXIT_SUCCESS;
}

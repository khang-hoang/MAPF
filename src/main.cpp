#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include "config.hpp"
#include "Map.hpp"
#include "ResourcePath.hpp"
#include "Color.hpp"

int main(int argc, char const **argv) {
    config::setExecutePath(argv[0]);

    const int32_t MAP_WIDTH = 500;
    const int32_t MAP_HEIGHT = 500;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1366, 768), "SFML window");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(getResourcePath("icon.png"))) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Start the game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Clear screen
        window.clear(Color::White);

        // Update the window
        window.display();
    }
    return EXIT_SUCCESS;
}

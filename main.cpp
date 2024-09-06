#include <SFML/Graphics.hpp>
#include <iostream>
#include "Editor.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Game Editor with ImGui Docking and Linked Windows");
    window.setFramerateLimit(60);

    Editor editor(window);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            editor.ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        // Render
        editor.Update();
        window.clear();
        editor.Render();
        window.display();
    }
    editor.Close();
    return 0;
}
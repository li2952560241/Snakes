#include <SFML/Graphics.hpp>
#include "include/game.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), L"贪吃蛇");
    Game game(window);
     while (window.isOpen()) {
            sf::Event event{};
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        std::cout << "success exit" << std::endl;
                        window.close();
                        break;
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Escape) {
                            window.close();
                        }
                        break;
                    default:
                        break;
                }
            }
            game.drawgame();
     }
    return 0;
}

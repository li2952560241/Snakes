#include <SFML/Graphics.hpp>
#include <iostream>
#ifndef GAME_HPP
#define GAME_HPP

class Game {
private:
    sf::RenderWindow& window;

public:
    Game(sf::RenderWindow& window) : window(window) {
       
    }

    void drawgame() {
        sf::RectangleShape rectangle(sf::Vector2f(10.f, 10.f));
        double frameLength = 15.0;
        
        auto size = window.getSize();
        rectangle.setPosition(frameLength, frameLength); //设置起点
        rectangle.setSize(sf::Vector2f(size.x - frameLength * 2, size.y - frameLength * 2));//这个是长方形的大小
        rectangle.setFillColor(sf::Color(230, 230, 230));
        rectangle.setOutlineThickness(frameLength); //边框的大小
        rectangle.setOutlineColor(sf::Color(15, 50, 110));

        //用长方形当作一个分界线
        sf::RectangleShape rectangle1(sf::Vector2f(10.f, 10.f));
        rectangle1.setPosition(size.x * 0.3, frameLength); //设置起点
        rectangle1.setSize(sf::Vector2f( frameLength , size.y - frameLength * 2 ));
        rectangle1.setFillColor(sf::Color(15, 50, 110));
        window.clear();
        window.draw(rectangle);
        window.draw(rectangle1);//分界线
        window.display();
        }
    
};

#endif 

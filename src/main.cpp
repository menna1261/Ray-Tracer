#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include<iostream>
using namespace std;

const int RAY_NUM = 300; 
const float PI = 3.14159265f;

int main() {
    const int width = 1100, height = 760;
    sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");

    // Create image -> texture -> sprite
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);

    // Create a circle
    sf::CircleShape circle(20);
    circle.setFillColor(sf::Color::White);
    circle.setOrigin({5, 5});

    sf::Vector2f dragOffset ;
    bool isDragging = false;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Handle window close
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle escape key press
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            // Handle Dragging Event
            if (event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                float MouseX = float(sf::Mouse::getPosition(window).x);
                float MouseY =  float(sf::Mouse::getPosition(window).y);

                sf::Vector2f MousePos = {MouseX ,MouseY};
                
                //Check If we're actually on the circle
                if(circle.getGlobalBounds().contains({MouseX,MouseY})){
                    isDragging = true;
                    dragOffset = circle.getPosition()- MousePos;
                }
            }

            if(event.type == sf::Event::MouseButtonReleased ){

                isDragging = false;
            }

            if(isDragging){

                float MouseX = float(sf::Mouse::getPosition(window).x);
                float MouseY =  float(sf::Mouse::getPosition(window).y);

                sf::Vector2f MousePos = {MouseX ,MouseY};
                circle.setPosition(MousePos + dragOffset);
            }
        }
        window.clear();
        window.draw(sprite);
        for(int i =0 ; i<RAY_NUM ;i++){

            float angle = (2*PI / RAY_NUM )*i;
    
            sf::Vector2f direction = sf::Vector2f(std::cos(angle) , std::sin(angle));
    
            for(float step = 0 ; step <1000 ; step+=1.0f){
    
                sf::Vector2f point = circle.getPosition() + direction * step;
                sf::Vertex pixel(point, sf::Color::Yellow);
                window.draw(&pixel, 1, sf::Points);
            }
    
        }
        window.draw(circle);
        window.display();
    }
}
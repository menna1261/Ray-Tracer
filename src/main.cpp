#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include<iostream>
using namespace std;

const int RAY_NUM = 300; 
const float PI = 3.14159265f;


bool isColliding(sf::Vector2f point , sf::CircleShape circle2 ){
    
    sf::Vector2f circle2Pos = circle2.getPosition(); // center
    float radius2 = circle2.getRadius();

    sf::Vector2f diff = point - circle2Pos;
    float distanceSquared = diff.x * diff.x + diff.y * diff.y;
    float radiusSquared = radius2 * radius2;

    if (distanceSquared <= radiusSquared) {
        return true;
}
    return false;
    
}



int main() {
    const int width = 1100, height = 760;
    sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");

    // Create image -> texture -> sprite
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);

    // Create a light source
    sf::CircleShape circle(20);
    circle.setFillColor(sf::Color::White);
    circle.setOrigin({20,20});
    circle.setPosition({432,231});

    //Create an object 
    sf::CircleShape circle2 (80);
    circle2.setFillColor(sf::Color::White);
    circle2.setOrigin({80,80});
    circle2.setPosition({659,346});

    sf::Vector2f dragOffset ;
    bool isDragging = false;
    bool isDragging2 = false;


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

                if(circle2.getGlobalBounds().contains({MouseX,MouseY})){
                    isDragging2 = true;
                    dragOffset = circle2.getPosition()-MousePos;
                }
            }

            if(event.type == sf::Event::MouseButtonReleased ){

                isDragging = false;
                isDragging2 = false;
            }

            float MouseX = float(sf::Mouse::getPosition(window).x);
            float MouseY =  float(sf::Mouse::getPosition(window).y);

            sf::Vector2f MousePos = {MouseX ,MouseY};

            if(isDragging){
                circle.setPosition(MousePos + dragOffset);
                std::cout<<"lightX : "<<circle.getPosition().x <<" "<<"lightY : "<<circle.getPosition().y<<std::endl;
            }

            if (isDragging2){
                circle2.setPosition(MousePos + dragOffset);
                std::cout<<"tX : "<<circle2.getPosition().x <<" "<<"tY : "<<circle2.getPosition().y<<std::endl;
            }
        }
        window.clear();
        window.draw(sprite);
        for(int i =0 ; i<RAY_NUM ;i++){

            float angle = (2*PI / RAY_NUM )*i;
    
            sf::Vector2f direction = sf::Vector2f(std::cos(angle) , std::sin(angle));
    
            for(float step = 0 ; step <1500 ; step+=1.0f){
    
                sf::Vector2f point = circle.getPosition() + direction * step;

                if(!isColliding(point , circle2))
                {
                    sf::Vertex pixel(point, sf::Color::Yellow);
                    window.draw(&pixel, 1, sf::Points); 
                }
                else
                {
                    break;
                }              
            }
    
        }
        window.draw(circle);
        window.draw(circle2);
        window.display();
    }

}

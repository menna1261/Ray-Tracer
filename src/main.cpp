#include <iostream>
#include <fstream>
#include<cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace std;

const int RAY_NUM = 300;
const float PI = 3.14159265f;
const int width = 1100, height = 760;
sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");

//Check if the ray's pixel is colliding with the circle2
bool isColliding(sf::Vector2f point, sf::CircleShape circle2)
{
    sf::Vector2f circle2Pos = circle2.getPosition(); // center
    float radius2 = circle2.getRadius();
    sf::Vector2f diff = point - circle2Pos;
    float distanceSquared = diff.x * diff.x + diff.y * diff.y;
    float radiusSquared = radius2 * radius2;

    if (distanceSquared <= radiusSquared)
    {
        return true;
    }
    return false;
}

void DrawSphere(sf::Vector2f SpherePos , sf::Vector2f lightPos){

    float SphereRaduis = 80;
    
    //Iterate over a bounding box around the sphere 
    for(int i = -SphereRaduis ; i<= SphereRaduis ;i++){
        for(int j = -SphereRaduis ; j<=SphereRaduis;j++){

            sf::Vector2f pixelPos = SpherePos + sf::Vector2f(i,j);
            float distanceFromCenter = std::sqrt(i * i + j * j);

           //Determine if the pixel is inside the sphere 
            if(distanceFromCenter <= SphereRaduis){
                
                //Normalize the vector 
                sf::Vector2f normal = sf::Vector2f(i, j) / distanceFromCenter; 
                if (distanceFromCenter == 0) normal = sf::Vector2f(0, 0);

                //Calculate the light direction for each pixel
                sf::Vector2f lightDir = lightPos - pixelPos;
                float lightDistance = sqrt((lightDir.x * lightDir.x) + (lightDir.y * lightDir.y));
                if (lightDistance > 0) lightDir /= lightDistance;  //Normalize the vector

                // Calculate diffuse lighting (dot product of normal and light direction)
                float diffuse = std::max(0.f, normal.x * lightDir.x + normal.y * lightDir.y);

                // Add ambient light to prevent the sphere from being completely black
                float ambient = 0.2f; 
                float intensity = ambient + (1.0f - ambient) * diffuse;

                // Set the pixel color based on the intensity
                sf::Color color = sf::Color(255 * intensity, 255 * intensity, 255 * intensity);
                sf::Vertex pixel(pixelPos, color);
                window.draw(&pixel, 1, sf::Points);

            }
        }
    }
    
}


int main()
{

    // Create image -> texture -> sprite
    sf::Image image;
    image.create(width, height, sf::Color::Black);
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    // Create a light source
    sf::CircleShape circle(20);
    circle.setFillColor(sf::Color::White);
    circle.setOrigin({20, 20});
    circle.setPosition({432, 231});

    // Create an object
    sf::CircleShape circle2(80);
    circle2.setFillColor(sf::Color::White);
    circle2.setOrigin({80, 80});
    circle2.setPosition({659, 346});

    sf::Vector2f dragOffset;
    bool isDragging = false;
    bool isDragging2 = false;

    //Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Handle window close
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle escape key press
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            // Handle Dragging Event
            if (event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                float MouseX = float(sf::Mouse::getPosition(window).x);
                float MouseY = float(sf::Mouse::getPosition(window).y);

                sf::Vector2f MousePos = {MouseX, MouseY};

                // Check If we're actually on the circle
                if (circle.getGlobalBounds().contains({MouseX, MouseY}))
                {
                    isDragging = true;
                    dragOffset = circle.getPosition() - MousePos;
                }

                if (circle2.getGlobalBounds().contains({MouseX, MouseY}))
                {
                    isDragging2 = true;
                    dragOffset = circle2.getPosition() - MousePos;
                }
            }

            //Checkk If we are dropping the circle
            if (event.type == sf::Event::MouseButtonReleased)
            {
                isDragging = false;
                isDragging2 = false;
            }

            float MouseX = float(sf::Mouse::getPosition(window).x);
            float MouseY = float(sf::Mouse::getPosition(window).y);

            sf::Vector2f MousePos = {MouseX, MouseY};

            if (isDragging)
            {
                circle.setPosition(MousePos + dragOffset);
            }

            if (isDragging2)
            {
                circle2.setPosition(MousePos + dragOffset);
            }
        }

        window.clear();
        window.draw(sprite);

        //Drawing the Rays out of the light source
        for (int i = 0; i < RAY_NUM; i++)
        {
            float angle = (2 * PI / RAY_NUM) * i;
            sf::Vector2f direction = sf::Vector2f(std::cos(angle), std::sin(angle));

            for (float step = 0; step < 1500; step += 1.0f)
            {
                sf::Vector2f point = circle.getPosition() + direction * step;

                if (!isColliding(point, circle2))
                {
                    sf::Vertex pixel(point, sf::Color::Yellow);
                    //To make it blurry like real light 
                    //sf::Vertex pixel2({point.x +5 , point.y+5}, sf::Color::White);
                    //sf::Vertex pixel3({point.x -5 , point.y-5}, sf::Color::White);
                    window.draw(&pixel, 1, sf::Points);
                    //window.draw(&pixel2, 1, sf::Points);
                    //window.draw(&pixel3, 1, sf::Points);

                }
 
                else
                {
                    break;
                }
            }
        }
        window.draw(circle);
        window.draw(circle2);
        DrawSphere({678,237},circle.getPosition());
        window.display();
    }
}

#include <iostream>
#include <fstream>
#include<cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
// just for the time counter
#include <chrono>
using namespace std;

#define RAYTRACER true
const int RAY_NUM = 300;
#define PI 3.14159265358979323846f
const int width = 1100, height = 760;
sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");

typedef struct FRAME_RATE_INFO {
    int frames;
    double  delta_time;
} FRAME_RATE_INFO;


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

// TBH, I didn't write this function, I asked chatGPT to write it for me
bool is_in_shadow_cone(const sf::Vector2f& pixel, const sf::Vector2f& light_position, const sf::Vector2f& shadow_position, float shadow_radius)
{
    // Direction from light to shadow center
    sf::Vector2f light_to_shadow = shadow_position - light_position;


    float distance_to_shadow = std::sqrt(light_to_shadow.x * light_to_shadow.x + light_to_shadow.y * light_to_shadow.y);
    if (distance_to_shadow == 0.0f) return false; // avoid division by zero

    // Normalize
    sf::Vector2f dir = light_to_shadow / distance_to_shadow;

    // Get perpendicular vectors (for left/right edge of the shadow object)
    sf::Vector2f perp(-dir.y, dir.x);
    sf::Vector2f edge_left = shadow_position + perp * shadow_radius;
    sf::Vector2f edge_right = shadow_position - perp * shadow_radius;

    // Vectors from light to the left/right edges
    sf::Vector2f to_left = edge_left - light_position;
    sf::Vector2f to_right = edge_right - light_position;
    sf::Vector2f to_pixel = light_position  - pixel;

    // Cross products to check if point is inside the cone
    float cross1 = to_left.x * to_pixel.y - to_left.y * to_pixel.x;
    float cross2 = to_pixel.x * to_right.y - to_pixel.y * to_right.x;

    bool is_with_in_cone = cross1 >= 0 && cross2 >= 0;

    // Also make sure the pixel is farther than the shadow object (behind it)
    float pixel_distance = std::sqrt((pixel.x - light_position.x) * (pixel.x - light_position.x) +
                                    (pixel.y - light_position.y) * (pixel.y - light_position.y));

    return is_with_in_cone && pixel_distance > distance_to_shadow;
}

// we shouldn't really be doing a shadow_object here, but for now we will just use a circle
// an array of the scene objects would be better and then we check collision for each object
void draw_light_emission(sf::CircleShape light_source, sf::CircleShape shadow_object, double light_strength = 1.0){
    //Drawing the Rays out of the light source

    //We can draw a big circle in a texture and then cut out the shadow parts
    // Step 1: Draw a circle to a RenderTexture
    sf::RenderTexture render_texture;
    render_texture.create(1024, 1024); // the higher the resolution the better, but also the slower
    
    sf::CircleShape light_emission(light_strength);
    light_emission.setPosition(0, 0);
    light_emission.setFillColor(sf::Color::Yellow);
    light_emission.setRadius(light_strength);
    light_emission.setPointCount(500); // number of points in the circle, results in smoother circle
    
    render_texture.clear(sf::Color::Transparent);
    render_texture.draw(light_emission, sf::RenderStates(sf::BlendAdd));
    render_texture.display();
    
    // Step 2: Get texture as image
    sf::Image image = render_texture.getTexture().copyToImage();

    // Step 3: Loop through every pixel in the image
    // and "Cut out" pixels

    // Prepare math
    sf::Vector2f light_position = light_source.getPosition();
    sf::Vector2f shadow_position = shadow_object.getPosition();
    float shadow_radius = shadow_object.getRadius();

    // Inverse transform
    sf::Transform inverse;
    inverse.translate(-light_position.x + light_strength, -light_position.y + light_strength);

    // Loop over all pixels
    for (unsigned int y = 0; y < image.getSize().y; ++y) {
        for (unsigned int x = 0; x < image.getSize().x; ++x) {
            sf::Vector2f local_pixel(x, y);
            sf::Vector2f global_pixel = inverse.getInverse().transformPoint(local_pixel);

            if (is_in_shadow_cone(global_pixel, light_position, shadow_position, shadow_radius)) {
                image.setPixel(x, y, sf::Color::Transparent);
            }
        }
    }
    
    // Step 4: Convert back to texture
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    sprite.setPosition(light_source.getPosition());
    sprite.setOrigin(light_strength, light_strength);
    

    window.draw(sprite, sf::RenderStates(sf::BlendAdd));



}

FRAME_RATE_INFO frame_rate_info = {0, 0};

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

    using clock = std::chrono::high_resolution_clock;
    auto previous = clock::now();
    auto fps_timer = previous;

    //Main loop
    while (window.isOpen())
    {
        
        {   // FPS Counter     
            
            
            auto now = clock::now();

            std::chrono::duration<double> delta = now - previous;
            previous = now;

            // Delta time in seconds
            frame_rate_info.delta_time = delta.count();

            frame_rate_info.frames++;
            if (std::chrono::duration<double>(now - fps_timer).count() >= 1.0) {
                std::cout << "FPS: " << frame_rate_info.frames << ", Frame time: " << frame_rate_info.delta_time * 1000.0 << " ms\n";
                frame_rate_info.frames = 0;
                fps_timer = now;
            }
        }


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

#if RAYTRACER == true
        // Step 1: a RenderTexture that covers the whole screen
        sf::RenderTexture render_texture;
        // use the screen width and height
        render_texture.create(width, height);
        
        render_texture.clear(sf::Color::Transparent);
        render_texture.display();
        // Step 2: Get texture as image
        sf::Image image = render_texture.getTexture().copyToImage();
        
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
                    if (point.x < 0 || point.x > width || point.y < 0 || point.y > height)
                    {
                        break;
                    }
                    image.setPixel(point.x, point.y, sf::Color::Yellow);

                    // blur using a for loop, be aware that this costs performance

                    for (int j = 0; j < 5; j++)
                    {
                        int offset = j;
                        if (point.x + offset < width && point.y + offset < height) {
                            image.setPixel(point.x + offset, point.y + offset, sf::Color::Yellow);
                        }
                        
                        if (point.x - offset > 0 && point.y - offset > 0) {
                            image.setPixel(point.x - offset, point.y - offset, sf::Color::Yellow);
                        }
                    }
                    
                }
                
                else
                {
                    break;
                }
            }
        }

        // Step 4: Convert back to texture
        sf::Texture texture;
        texture.loadFromImage(image);
        sf::Sprite sprite(texture);
        // the sprite cover the whole screen, no need to change the position
        sprite.setPosition(0,0);
        sprite.setOrigin(0, 0);
        

        window.draw(sprite, sf::RenderStates(sf::BlendAdd));
#else
        draw_light_emission(circle, circle2, 500);
#endif

        window.draw(circle);
        window.draw(circle2);
        DrawSphere({678,237},circle.getPosition());
        window.display();
    }
}

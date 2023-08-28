#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

#include <array>
#include <cmath>
#include <iostream>

#include "Entity.h"
#include "Map.h"
#include "MapPreviewRenderer.h"
#include "Maths.h"

struct Renderer
{
    std::vector<sf::Uint8> pixels;

    sf::Texture texture;
    sf::RectangleShape sprite;

    Renderer()
        : pixels(WINDOW_WIDTH * WINDOW_HEIGHT * 4)
    {
        texture.create(WINDOW_WIDTH, WINDOW_HEIGHT);
        sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
    }

    void clear()
    {
        std::memset(pixels.data(), 0, pixels.size());
    }

    void setPixel(int x, int y, sf::Color colour)
    {
        if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
        {
            return;
        }
        sf::Uint8* ptr = &pixels.at((y * WINDOW_WIDTH + x) * 4);
        ptr[0] = colour.r;
        ptr[1] = colour.g;
        ptr[2] = colour.b;
        ptr[3] = 255;
    }

    void render(sf::RenderTarget& target)
    {
        sprite.setTexture(&texture);
        texture.update(pixels.data());
        target.draw(sprite);
    }
};

int main()
{
    sf::RenderWindow window({WINDOW_WIDTH, WINDOW_HEIGHT}, "WalksoN", sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    Camera camera;
    camera.position = {MAP_X_SIZE / 2, MAP_Y_SIZE / 2};
    camera.direction = {1, 0};

    // The plane is perpendicular to the camera's direction vector
    camera.plane = sf::Vector2f{camera.direction.y, camera.direction.x};

    Map map;
    PreviewRender preview_renderer(map);

    Renderer renderer;

    while (window.isOpen())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // camera.direction = clamp_angle(camera.direction);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            camera.position += camera.direction * 0.2f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            camera.position -= camera.direction * 0.2f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            camera.direction = Maths::rotate(camera.direction, -3.0f);
            camera.plane = sf::Vector2f{camera.direction.y, -camera.direction.x};
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            camera.direction = Maths::rotate(camera.direction, 3.0f);
            camera.plane = sf::Vector2f{camera.direction.y, -camera.direction.x};
        }

        window.clear();
        renderer.clear();

        for (int x = 0; x < window.getSize().x; x++)
        {
            auto start_direction = camera.direction - camera.plane;
            int middle = window.getSize().y / 2;

            // Ceiling
            for (int y = 0; y < middle; y++)
            {
                renderer.setPixel(x, y, {50, 150, 230});
            }

            // Floor
            for (int y = middle; y < window.getSize().y; y++)
            {
                renderer.setPixel(x, y, {100, 200, 80});
            }
        }

        renderer.render(window);
        preview_renderer.render_preview(window, camera);

        window.display();
    }
}

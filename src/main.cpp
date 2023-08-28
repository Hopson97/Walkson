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
            camera.plane = Maths::perpendicular(-camera.direction);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            camera.direction = Maths::rotate(camera.direction, 3.0f);
            camera.plane = Maths::perpendicular(-camera.direction);
        }

        window.clear();
        renderer.clear();

        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            // Convert to device coords such that:
            // camera_x == -1 when x == 0, and
            // camera_x == 1  when x == 1
            float camera_x = 2 * x / (float)WINDOW_WIDTH - 1;

            // The direction the ray is facing for this
            auto ray_direction = camera.direction + camera.plane * camera_x;

            // Calculate the position of the player in the world
            auto pos = Maths::cast_vector<int>(camera.position); //   *TILE_SIZE;

            // Difference between one x/y side of the grid to the next x/y
            sf::Vector2f delta_dist;
            delta_dist.x = std::abs(1.0f / ray_direction.x);
            delta_dist.y = std::abs(1.0f / ray_direction.y);

            bool hit_wall = false;
            int side = 0;

            // Length of ray from cam postion to next x/y side
            sf::Vector2f side_dist;

            // Current step direction
            sf::Vector2i step;

            // Calculate step and initial sideDist
            if (ray_direction.x < 0)
            {
                step.x = -1;
                side_dist.x = (camera.position.x - pos.x) * delta_dist.x;
            }
            else
            {
                step.x = 1;
                side_dist.x = (pos.x + 1.0 - camera.position.x) * delta_dist.x;
            }
            if (ray_direction.y < 0)
            {
                step.y = -1;
                side_dist.y = (camera.position.y - pos.y) * delta_dist.y;
            }
            else
            {
                step.y = 1;
                side_dist.y = (pos.y + 1.0 - camera.position.y) * delta_dist.y;
            }

            for (int i = 0; i < 100 && !hit_wall; i++)
            {
                // Jump to next x/y square
                if (side_dist.x < side_dist.y)
                {
                    side_dist.x += delta_dist.x;
                    pos.x += step.x;
                    side = 0;
                }
                else
                {
                    side_dist.y += delta_dist.y;
                    pos.y += step.y;
                    side = 1;
                }

                if (map.get_tile(pos.x, pos.y) > 0)
                {
                    hit_wall = true;
                }
            }

            float distance_to_wall =
            side == 0 ? side_dist.x - delta_dist.x : side_dist.y - delta_dist.y;

            int wall_height = WINDOW_HEIGHT / distance_to_wall;

            int start = WINDOW_HEIGHT / 2 - wall_height / 2;
            int end = WINDOW_HEIGHT / 2 + wall_height / 2;

            if (start < 0)
                start = 0;
            if (end >= WINDOW_HEIGHT)
                end = WINDOW_HEIGHT - 1;

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

            // Wall 
            if (hit_wall)
            for (int y = start; y < end; y++)
            {
                renderer.setPixel(x, y, sf::Color::Red);
            }


        }

        renderer.render(window);
        preview_renderer.render_preview(window, camera);

        window.display();
    }
}

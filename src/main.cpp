#include <SFML/Graphics/RenderTexture.hpp>
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
    sf::RectangleShape pixel_sprite;

    sf::RectangleShape clear_sprite;

    Renderer()
        : pixels(WINDOW_WIDTH * WINDOW_HEIGHT * 4)
    {
        texture.create(WINDOW_WIDTH, WINDOW_HEIGHT);
        pixel_sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
        clear_sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT / 2});
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

    void render(sf::RenderTarget& target, bool inside)
    {
        float middle = WINDOW_HEIGHT / 2.0f;

        sf::Color ground = inside ? FLOOR : sf::Color{100, 200, 80};
        sf::Color sky = inside ? sf::Color{200, 200, 180} : sf::Color{50, 150, 230};

        // Draw the "sky"
        clear_sprite.setPosition({0, middle});
        clear_sprite.setFillColor(sky);
        target.draw(clear_sprite);

        // Draw the ground
        clear_sprite.setPosition({0, 0});
        clear_sprite.setFillColor(ground);
        target.draw(clear_sprite);

        clear_sprite.setPosition({0, 0});
        clear_sprite.setSize({10, 10});
        clear_sprite.setFillColor(sf::Color::Red);
        target.draw(clear_sprite);

        pixel_sprite.setTexture(&texture);
        texture.update(pixels.data());
        target.draw(pixel_sprite);

        clear_sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT / 2});
    }
};

int main()
{
    sf::RenderWindow window({1280, 720}, "WalksoN", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);

    sf::RenderTexture renderTexture;
    renderTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    sf::RectangleShape framebuffer({960, 540});

    Map map;
    map.load("data/map");
    PreviewRender preview_renderer(map);

    Renderer renderer;

    Camera camera;
    camera.position = Maths::cast_vector<float>(map.start);
    camera.direction = {1, 0};

    // The plane is perpendicular to the camera's direction vector
    camera.plane = sf::Vector2f{camera.direction.y, camera.direction.x};

    while (window.isOpen())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (e.type == sf::Event::KeyPressed)
            {
                auto key = e.key.code;
                if (key == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        float SPEED = 0.2f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            camera.position += camera.direction * SPEED;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            camera.position -= camera.direction * SPEED;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            camera.position += Maths::perpendicular(camera.direction) * SPEED;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            camera.position += Maths::perpendicular(-camera.direction) * SPEED;
        }

        static auto last_mouse_pos = sf::Mouse::getPosition().x;
        auto this_frame_mouse_pos = sf::Mouse::getPosition().x;

        float diff = (this_frame_mouse_pos - last_mouse_pos) / 4.0f;
        camera.direction = Maths::rotate(camera.direction, diff);
        camera.plane = Maths::perpendicular(-camera.direction);
        sf::Mouse::setPosition({(int)window.getSize().x / 2, (int)window.getSize().y / 2},
                               window);
        last_mouse_pos = sf::Mouse::getPosition().x;

        window.clear();
        renderTexture.clear();
        renderer.clear();

        // If the player is touching a "door", then warp them to other side
        auto i_camera_pos = Maths::cast_vector<int>(camera.position);
        auto tile = map.get_tile(i_camera_pos.x, i_camera_pos.y);
        while (tile == '1')
        {
            camera.position += camera.direction;
            i_camera_pos = Maths::cast_vector<int>(camera.position);
            tile = map.get_tile(i_camera_pos.x, i_camera_pos.y);
        }

        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            // Convert to device coords such that:
            // camera_x == -1 when x == 0, and
            // camera_x == 1  when x == 1
            float camera_x = 2 * x / (float)WINDOW_WIDTH - 1;

            // The direction the ray is facing for this
            auto ray_direction = camera.direction + camera.plane * camera_x;

            // Calculate the position of the player in the world
            auto pos = i_camera_pos; //   *TILE_SIZE;

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
                side_dist.x = (pos.x + 1.0f - camera.position.x) * delta_dist.x;
            }
            if (ray_direction.y < 0)
            {
                step.y = -1;
                side_dist.y = (camera.position.y - pos.y) * delta_dist.y;
            }
            else
            {
                step.y = 1;
                side_dist.y = (pos.y + 1.0f - camera.position.y) * delta_dist.y;
            }

            // DDA algorthim
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

                if (map.get_tile(pos.x, pos.y) != '.' &&
                    map.get_tile(pos.x, pos.y) != ',')
                {
                    hit_wall = true;
                }
            }

            // Distance from the camera plane to the wall
            float distance_to_wall =
                side == 0 ? side_dist.x - delta_dist.x : side_dist.y - delta_dist.y;

            {

                // Calculate the height of the wall
                int wall_height = WINDOW_HEIGHT / distance_to_wall;
                int start = WINDOW_HEIGHT / 2 - wall_height / 2;
                int end = WINDOW_HEIGHT / 2 + wall_height / 2;
                if (start < 0)
                    start = 0;
                if (end >= WINDOW_HEIGHT)
                    end = WINDOW_HEIGHT - 1;

                // R E N D E R I N G

                // Wall
                if (hit_wall)
                {

                    for (int y = start; y < end; y++)
                    {
                        sf::Uint8 r = rand() % 255;
                        sf::Uint8 g = rand() % 255;
                        sf::Uint8 b = rand() % 255;
                        renderer.setPixel(x, y,
                                          tile_to_colour(map.get_tile(pos.x, pos.y)));
                    }
                }
            }
        }

        renderer.render(renderTexture, tile == ',');
        framebuffer.setTexture(&renderTexture.getTexture());
        window.draw(framebuffer);

        // preview_renderer.render_preview(window, camera);

        window.display();
    }
}

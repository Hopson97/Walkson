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

    void set_pixel(int x, int y, sf::Color colour)
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

    void draw_vertical_line(int x, int y_start, int y_end, sf::Color colour)
    {
        for (int y = y_start; y < y_end; y++)
        {
            set_pixel(x, y, colour);
        }
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

        // Draw the pixels ontop
        pixel_sprite.setTexture(&texture);
        texture.update(pixels.data());
        target.draw(pixel_sprite);

        clear_sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT / 2});
    }
};

sf::Color operator/=(sf::Color& colour, int mod)
{
    colour.r /= mod;
    colour.g /= mod;
    colour.b /= mod;
    return colour;
}

auto gen_texture()
{
    sf::Image image;
    image.create(64, 64);

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            sf::Color c;
            c.r = y + x;
            c.g = y / (x + 1);
            c.g = (x ^ y) * 3;
            image.setPixel(x, y, c);
        }
    }

    return image;
}

int main()
{
    sf::RenderWindow window({1280, 720}, "WalksoN", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);

    window.setPosition({1960 - 1280, 0});

    sf::RenderTexture renderTexture;
    renderTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::RectangleShape framebuffer({WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2});

    auto texture = gen_texture();
    sf::RectangleShape test_texture_test(Maths::cast_vector<float>(texture.getSize()));
    sf::Texture sftex;
    sftex.loadFromImage(texture);
    test_texture_test.setTexture(&sftex);

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
        sf::Vector2f movement;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            movement += camera.direction * SPEED;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            movement -= camera.direction * SPEED;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            movement += Maths::perpendicular(camera.direction) * SPEED;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            movement += Maths::perpendicular(-camera.direction) * SPEED;
        }

        auto next = camera.position + movement;
        auto next_tile = map.get_tile(next.x, next.y);

        // Naive collision resolution
        if (next_tile == '#')
        {
            if (std::abs(movement.x) > 0)
            {
                movement.x = 0;
            }
            if (std::abs(movement.y) > 0)
            {
                movement.y = 0;
            }
        }
        camera.position += movement;

        // If the player is touching a "door", then warp them to other side
        auto i_camera_pos = Maths::cast_vector<int>(camera.position);
        auto tile = map.get_tile(i_camera_pos.x, i_camera_pos.y);

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

        // Do the rendering
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            // Convert to device coords (so -1 left & bottom, and 1 top && right
            float camera_x = 2 * x / (float)WINDOW_WIDTH - 1;

            // The direction the ray is facing for this slice of the window
            auto ray_direction = camera.direction + camera.plane * camera_x;

            // Calculate the position of the player in the world
            auto pos = i_camera_pos; //   *TILE_SIZE;

            // Difference between one x/y side of the grid to the next x/y
            sf::Vector2f delta_dist{std::fabs(1.0f / ray_direction.x),
                                    std::fabs(1.0f / ray_direction.y)};

            int side = 0;

            // Length of ray from cam postion to next x/y side
            // Calculate the "quadrant" being faced
            sf::Vector2f side_dist = {
                (ray_direction.x < 0 ? (camera.position.x - pos.x)
                                     : (pos.x + 1.0f - camera.position.x)),
                (ray_direction.y < 0 ? (camera.position.y - pos.y)
                                     : (pos.y + 1.0f - camera.position.y)),
            };
            side_dist.x *= delta_dist.x;
            side_dist.y *= delta_dist.y;

            // Current step direction
            sf::Vector2i step = {
                ray_direction.x < 0 ? -1 : 1,
                ray_direction.y < 0 ? -1 : 1,
            };

            // Calculate step and initial sideDist

            // DDA algorthim
            bool hit_wall = false;
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

                auto tile = map.get_tile(pos.x, pos.y);
                auto colour = tile_to_colour(tile);
                if (side == 1)
                {
                    colour /= 2;
                }
                renderer.draw_vertical_line(x, start, end, colour);

                if (tile == '@')
                {
                    start = end;
                    end = WINDOW_HEIGHT / 2 + wall_height;
                    colour /= 2;
                    renderer.draw_vertical_line(x, start, end, colour);
                }
            }
        }

        renderer.render(renderTexture, tile == ',');
        framebuffer.setTexture(&renderTexture.getTexture());
        window.draw(framebuffer);

        window.draw(test_texture_test);

        preview_renderer.render_preview(window, camera);

        window.display();
    }
}

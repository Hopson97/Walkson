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
#include "Renderer.h"

namespace
{
    sf::Color operator/=(sf::Color& colour, int mod)
    {
        colour.r /= mod;
        colour.g /= mod;
        colour.b /= mod;
        return colour;
    }

    sf::Vector2f get_keyboard_input(const sf::Vector2f& direction)
    {
        float SPEED = 0.1f;

        sf::Vector2f movement;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            movement += direction * SPEED;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            movement -= direction * SPEED;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            movement += Maths::perpendicular(direction) * SPEED;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            movement += Maths::perpendicular(-direction) * SPEED;
        }

        return movement;
    }

    void do_raycast(int x, const Camera& camera, const sf::Vector2i& i_camera_pos,
                    const Map& map, Renderer& renderer)
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

            if (map.get_tile(pos.x, pos.y) != '.' && map.get_tile(pos.x, pos.y) != ',')
            {
                hit_wall = true;
            }
        }

        // Distance from the camera plane to the wall
        float distance_to_wall =
            side == 0 ? side_dist.x - delta_dist.x : side_dist.y - delta_dist.y;

        // Get info about the "hit" tile
        auto tile = map.get_tile(pos.x, pos.y);
        auto tile_info = get_tile_info(tile);

        // Calculate the height of the wall as a ratio of distance to window height
        int wall_height = WINDOW_HEIGHT / distance_to_wall;
        int start = WINDOW_HEIGHT / 2 - wall_height / 2;
        int end = WINDOW_HEIGHT / 2 + (wall_height / 2 * tile_info.height);
        if (start < 0)
            start = 0;
        if (end >= WINDOW_HEIGHT)
            end = WINDOW_HEIGHT - 1;

        // R E N D E R I N G
        // Wall
        if (hit_wall)
        {
            auto colour = tile_info.colour;
            colour.r = std::max(0, (int)colour.r - (int)distance_to_wall * 5);
            colour.g = std::max(0, (int)colour.g - (int)distance_to_wall * 5);
            colour.b = std::max(0, (int)colour.b - (int)distance_to_wall * 5);
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
} // namespace

enum class RenderStyle
{
    Normal,
    PerLine,
};

int main()
{
    sf::RenderWindow window({WINDOW_WIDTH * 3, WINDOW_HEIGHT * 3},
                            "Walkson - Press L to unlock mouse - Press ESC to exit",
                            sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);

    window.setPosition({1960 - (int)(window.getSize().x * 1.1), 0});

    sf::RenderTexture render_texture;
    render_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::RectangleShape framebuffer({
        (float)window.getSize().x,
        (float)window.getSize().y,
    });

    Map map;
    map.load("assets/map");
    PreviewRender preview_renderer(map);

    Renderer renderer;

    Camera camera;
    camera.position = Maths::cast_vector<float>(map.start);
    camera.direction = {1, 0};

    RenderStyle render_style = RenderStyle::Normal;
    int current_x = 0;

    auto clear_all = [&]()
    {
        window.clear();
        render_texture.clear();
        renderer.clear();
    };

    // The plane is perpendicular to the camera's direction vector
    camera.plane = Maths::perpendicular(camera.direction);
    bool mouse_enabled = true;
    while (window.isOpen())
    {
        window.setMouseCursorVisible(!mouse_enabled);
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

                else if (key == sf::Keyboard::L)
                {
                    mouse_enabled = !mouse_enabled;
                }

                else if (key == sf::Keyboard::Q)
                {
                    clear_all();
                    render_style = RenderStyle::PerLine;
                    current_x = 0;
                }
            }
        }

        // Enabling input when rendering per-line can cause issues
        if (render_style == RenderStyle::Normal)
        {
            //  ---------------------
            //      Keyboard input
            //  ---------------------
            auto movement = get_keyboard_input(camera.direction);
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

            //  ---------------------
            //      Mouse input
            //  ---------------------
            if (mouse_enabled)
            {

                static auto last_mouse_pos = sf::Mouse::getPosition().x;
                auto this_frame_mouse_pos = sf::Mouse::getPosition().x;

                float diff = (this_frame_mouse_pos - last_mouse_pos) / 4.0f;
                camera.direction = Maths::rotate(camera.direction, diff);
                camera.plane = Maths::perpendicular(-camera.direction);
                sf::Mouse::setPosition(
                    {(int)window.getSize().x / 2, (int)window.getSize().y / 2}, window);
                last_mouse_pos = sf::Mouse::getPosition().x;
            }
        }

        //  ---------------------
        //      Pre-Render
        //  ---------------------
        if (render_style == RenderStyle::Normal)
        {
            clear_all();
        }

        //  ---------------------
        //      Rendering
        //  ---------------------
        auto i_camera_pos = Maths::cast_vector<int>(camera.position);
        auto tile = map.get_tile(i_camera_pos.x, i_camera_pos.y);

        if (render_style == RenderStyle::Normal)
        {
            for (int x = 0; x < WINDOW_WIDTH; x++)
            {
                do_raycast(x, camera, i_camera_pos, map, renderer);
            }
        }
        else if (render_style == RenderStyle::PerLine)
        {
            do_raycast(current_x++, camera, i_camera_pos, map, renderer);
            if (current_x == WINDOW_WIDTH - 1)
            {
                render_style = RenderStyle::Normal;
            }
        }

        renderer.render(render_texture, tile == ',');
        framebuffer.setTexture(&render_texture.getTexture());
        window.draw(framebuffer);

        preview_renderer.render_preview(window, camera);

        window.display();
    }
}

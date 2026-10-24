#include "Renderer.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include "Constants.h"

Renderer::Renderer()
    : pixels(WINDOW_WIDTH * WINDOW_HEIGHT * 4)
    , texture(sf::Vector2u{WINDOW_WIDTH, WINDOW_HEIGHT})
{
    pixel_sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
    clear_sprite.setSize({WINDOW_WIDTH, WINDOW_HEIGHT / 2});
}

void Renderer::clear()
{
    std::memset(pixels.data(), 0, pixels.size());
}

void Renderer::set_pixel(int x, int y, sf::Color colour)
{
    if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
    {
        return;
    }
    uint8_t* ptr = &pixels.at((y * WINDOW_WIDTH + x) * 4);
    ptr[0] = colour.r;
    ptr[1] = colour.g;
    ptr[2] = colour.b;
    ptr[3] = 255;
}

void Renderer::draw_vertical_line(int x, int y_start, int y_end, sf::Color colour)
{
    for (int y = y_start; y < y_end; y++)
    {
        set_pixel(x, y, colour);
    }
}

void Renderer::render(sf::RenderTarget& target, bool inside)
{
    float middle = WINDOW_HEIGHT / 2.0f;

    sf::Color ground = inside ? FLOOR.colour : sf::Color{100, 200, 80};
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
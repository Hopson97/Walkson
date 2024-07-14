#pragma once

#include <vector>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

struct Renderer
{
    std::vector<sf::Uint8> pixels;

    sf::Texture texture;
    sf::RectangleShape pixel_sprite;

    sf::RectangleShape clear_sprite;

    Renderer();

    void clear();
    void set_pixel(int x, int y, sf::Color colour);

    void draw_vertical_line(int x, int y_start, int y_end, sf::Color colour);

    void render(sf::RenderTarget& target, bool inside);
};
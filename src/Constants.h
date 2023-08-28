#pragma once

#include <cstdint>
#include <unordered_map>

#include <SFML/Graphics/Color.hpp>

constexpr int FIELD_OF_VIEW = 80;
constexpr int TILE_SIZE = 64;

constexpr int WINDOW_WIDTH = 400;
constexpr int WINDOW_HEIGHT = 222;


using Tile = char;

const sf::Color WALL = {100, 100, 100, 128};
const sf::Color GRASS = {50, 50, 50, 128};
const sf::Color FLOOR = {200, 150, 150, 128};

inline sf::Color tile_to_colour(Tile t)
{
    switch (t)
    {
        case '.':
            return GRASS;
        case ',':
            return FLOOR;
        case '#':
            return WALL;
    }

    return sf::Color::Magenta;
}
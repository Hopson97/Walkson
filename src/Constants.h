#pragma once

#include <cstdint>
#include <unordered_map>

#include <SFML/Graphics/Color.hpp>

constexpr int FIELD_OF_VIEW = 80;
constexpr int TILE_SIZE = 64;

constexpr int WINDOW_WIDTH = 400;
constexpr int WINDOW_HEIGHT = 222;

struct Tile
{
    sf::Color colour;
    float height = 0;
};

const Tile WALL = {.colour = {100, 100, 100, 128}, .height = 1.0f};
const Tile DOOR = {.colour = {255, 165, 0, 128}, .height = 0.9f};

const Tile GRASS = {.colour = {50, 50, 50, 128}, .height = 1.0f};
const Tile FLOOR = {.colour = {200, 150, 150, 128}, .height = 1.0f};

const Tile CASTLE_A = {.colour = {150, 150, 150, 128}, .height = 2.0f};
const Tile CASTLE_B = {.colour = {200, 200, 200, 128}, .height = 2.0f};

inline const Tile& get_tile_info(char t)
{
    switch (t)
    {
        case '#':
            return WALL;

        case '1':
            return DOOR;

        case '.':
            return GRASS;
        case ',':
            return FLOOR;

        case 'A':
            return CASTLE_A;

        case 'B':
            return CASTLE_B;
    }
    static Tile error{sf::Color::Magenta, 1.0f};
    return error;
}
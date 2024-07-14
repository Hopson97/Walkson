#pragma once

#include <vector>

#include <SFML/System/Vector2.hpp>

#include "Constants.h"

struct Map
{
    std::vector<Tile> m_grid;
    sf::Vector2i size;
    sf::Vector2i start;
    sf::Vector2i testing_location;

    bool load(const char* path);

    unsigned index(int x, int y) const
    {
        return y * size.x + x;
    }

    Tile get_tile(int x, int y) const
    {
        if (x < 0 || x >= size.x || y < 0 || y >= size.y)
        {
            return '.';
        }

        return m_grid.at(index(x, y));
    }
};
#pragma once

#include <array>
#include "Constants.h"

struct Map
{
    std::array<Tile, MAP_X_SIZE * MAP_Y_SIZE> m_grid;

    Map();

    unsigned index(int x, int y) const
    {
        return y * MAP_X_SIZE + x;
    }

    Tile get_tile(int x, int y) const
    {
        if (x < 0 || x >= MAP_X_SIZE || y < 0 || y >= MAP_Y_SIZE)
        {
            return 0;
        }

        return m_grid.at(index(x, y));
    }
};
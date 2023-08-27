#include "Map.h"

Map::Map()
{
    m_grid.fill(0);

    for (int y = 0; y < MAP_Y_SIZE; y++)
    {
        m_grid.at(index(0, y)) = 1;
        m_grid.at(index(MAP_X_SIZE - 1, y)) = 1;
    }

    for (int x = 0; x < MAP_X_SIZE; x++)
    {
        m_grid.at(index(x, 0)) = 1;
        m_grid.at(index(x, MAP_Y_SIZE - 1)) = 1;
    }
}
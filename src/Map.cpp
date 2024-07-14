#include "Map.h"

#include <fstream>
#include <string>


bool Map::load(const char* path)
{
    m_grid.clear();
    start = {0, 0};
    size = {0, 0};

    std::ifstream f;

    f.open(path);
    if (!f.is_open())
    {
        return false;
    }

    std::string line;
    while (std::getline(f, line))
    {
        size.x = line.length();
        size.y++;
           
        int x = 0;
        for (auto c : line)
        {
            x += 1;

            if (c == 'S' || c == 's')
            {
                start = {x, size.y};
                m_grid.push_back('.');
            }
            else if (c == 'W')
            {
                testing_location = {x, size.y};
                m_grid.push_back(c);
            }
            else
            {
                m_grid.push_back(c);
            }
        }
    }

    return true;
}

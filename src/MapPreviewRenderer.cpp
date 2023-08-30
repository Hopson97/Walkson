#include "MapPreviewRenderer.h"

#include <cassert>

#include "Entity.h"
#include "Map.h"
#include "Maths.h"

constexpr int PREVIEW_SCALE = 6;
constexpr float PREVIEW_TILE_SIZE = 4;

PreviewRender::PreviewRender(const Map& map)
    : mp_map(&map)
{
    auto T_SIZE = PREVIEW_TILE_SIZE;
    for (int y = 0; y < mp_map->size.y; y++)
    {
        for (int x = 0; x < mp_map->size.x; x++)
        {
            sf::Vertex quad[4];
            quad[0].position = {x * T_SIZE, y * T_SIZE};
            quad[1].position = {x * T_SIZE, y * T_SIZE + T_SIZE};
            quad[2].position = {x * T_SIZE + T_SIZE, y * T_SIZE + T_SIZE};
            quad[3].position = {x * T_SIZE + T_SIZE, y * T_SIZE};

            for (int i = 0; i < 4; i++)
            {
                quad[i].color = tile_to_colour(mp_map->get_tile(x, y));
                m_tiles.push_back(quad[i]);
            }
        }
    }
}

void PreviewRender::render_preview(sf::RenderTarget& render_target, const Camera& camera)
{
    assert(mp_map);

    // Draw the tile map
    render_target.draw(m_tiles.data(), m_tiles.size(), sf::PrimitiveType::Quads);

    // Draw the player "forward" line
    auto& pos = camera.position;
    auto& dir = camera.direction;
    auto& plane = camera.plane;
    set_line_colour(sf::Color::Red);
    {

        m_line[0].position = {pos.x * PREVIEW_TILE_SIZE, pos.y * PREVIEW_TILE_SIZE};

        m_line[1].position = {(pos.x * PREVIEW_TILE_SIZE) + dir.x * 10,
                              (pos.y * PREVIEW_TILE_SIZE) + dir.y * 10};
        render_target.draw(m_line.data(), m_line.size(), sf::PrimitiveType::Lines);
    }
       
    // Draw FOV lines
    set_line_colour(sf::Color::Blue);
    {
        m_line[1].position = {(pos.x * PREVIEW_TILE_SIZE) + (dir.x - plane.x) * 10,
                              (pos.y * PREVIEW_TILE_SIZE) + (dir.y - plane.y) * 10};
        render_target.draw(m_line.data(), m_line.size(), sf::PrimitiveType::Lines);
    }

    {
        m_line[1].position = {(pos.x * PREVIEW_TILE_SIZE) + (dir.x + plane.x) * 10,
                              (pos.y * PREVIEW_TILE_SIZE) + (dir.y + plane.y) * 10};
        render_target.draw(m_line.data(), m_line.size(), sf::PrimitiveType::Lines);
    }


}

void PreviewRender::set_line_colour(sf::Color colour)
{
    for (auto& v : m_line)
    {
        v.color = colour;
    }
}
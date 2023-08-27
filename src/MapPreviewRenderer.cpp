#include "MapPreviewRenderer.h"

#include <cassert>

#include "Entity.h"
#include "Map.h"
#include "Maths.h"

constexpr int PREVIEW_SCALE = 6;
constexpr float PREVIEW_TILE_SIZE = TILE_SIZE / PREVIEW_SCALE;

const sf::Color WALL = {100, 200, 250, 128};
const sf::Color FLOOR = {50, 50, 50, 128};

PreviewRender::PreviewRender(const Map& map)
    : mp_map(&map)
{
    m_quad.setSize({PREVIEW_TILE_SIZE, PREVIEW_TILE_SIZE});
    m_quad.setOutlineColor({0, 0, 0, 50});
    m_quad.setOutlineThickness(1.0f);
}

void PreviewRender::render_preview(sf::RenderTarget& render_target, const Entity& camera)
{
    assert(mp_map);

    for (int y = 0; y < MAP_Y_SIZE; y++)
    {
        for (int x = 0; x < MAP_X_SIZE; x++)
        {
            m_quad.setPosition(x * PREVIEW_TILE_SIZE, y * PREVIEW_TILE_SIZE);

            auto tile = mp_map->get_tile(x, y);
            switch (tile)
            {
                case 0:
                    m_quad.setFillColor(FLOOR);
                    break;

                case 1:
                    m_quad.setFillColor(WALL);
                    break;

                default:
                    m_quad.setFillColor(sf::Color::Magenta);
            }
            render_target.draw(m_quad);

        }
    }

    // Draw the player "forward" line
    set_line_colour(sf::Color::Red);
    render_camera_line(render_target, camera.position, camera.direction, 1.0);

    // Draw the player "Field of Vision" lines
    set_line_colour({255,255,255,50});
    float left = clamp_angle(camera.direction - FIELD_OF_VIEW / 2);
    float right = clamp_angle(camera.direction + FIELD_OF_VIEW / 2);
    render_camera_line(render_target, camera.position, left, 2);
    render_camera_line(render_target, camera.position, right, 2);

}

void PreviewRender::set_line_colour(sf::Color colour)
{
    for (auto& v : m_line)
    {
        v.color = colour;
    }
}

void PreviewRender::render_camera_line(sf::RenderTarget& render_target,
                                       const sf::Vector2f& start, float angle, float size)
{
    float delta_x = std::sin(rads(angle)) * PREVIEW_TILE_SIZE;
    float delta_y = std::cos(rads(angle)) * PREVIEW_TILE_SIZE;

    m_line[0].position = {start.x * PREVIEW_TILE_SIZE,
                          start.y * PREVIEW_TILE_SIZE};

    m_line[1].position = {start.x * PREVIEW_TILE_SIZE + delta_x * size,
                          start.y * PREVIEW_TILE_SIZE + delta_y * size};
    render_target.draw(m_line.data(), m_line.size(), sf::PrimitiveType::Lines);
}

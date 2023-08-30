#pragma once

#include <array>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Constants.h"

struct Map;
struct Camera;

struct PreviewRender
{
    std::array<sf::Vertex, 2> m_line;
    std::vector<sf::Vertex> m_tiles;

    const Map* mp_map = nullptr;

    PreviewRender(const Map& map);

    void render_preview(sf::RenderTarget& render_target, const Camera& camera);

    void set_line_colour(sf::Color colour);
};
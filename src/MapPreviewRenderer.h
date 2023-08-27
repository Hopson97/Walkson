#pragma once

#include <array>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Constants.h"

struct Map;
struct Entity;

struct PreviewRender
{
    constexpr static int PREVIEW_SCALE = 8;
    constexpr static float PREVIEW_TILE_SIZE = TILE_SIZE / PREVIEW_SCALE;

    std::array<sf::Vertex, 2> m_line;
    sf::RectangleShape m_quad;

    const Map* mp_map = nullptr;

    PreviewRender(const Map& map);

    void render_preview(sf::RenderTarget& render_target,
                        const Entity& camera);
};
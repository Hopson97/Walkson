#pragma once

#include <array>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Constants.h"

struct Map;
struct Entity;

struct PreviewRender
{
    std::array<sf::Vertex, 2> m_line;
    sf::RectangleShape m_quad;

    const Map* mp_map = nullptr;

    PreviewRender(const Map& map);

    void render_preview(sf::RenderTarget& render_target,
                        const Entity& camera);

    void set_line_colour(sf::Color colour);
    void render_camera_line(sf::RenderTarget& render_target, const sf::Vector2f& start,
                            float angle, float size);
};
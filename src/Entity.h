#pragma once

#include <SFML/System/Vector2.hpp>

struct Entity
{
    sf::Vector2f position;
    sf::Vector2f direction;
};

struct Camera : public Entity
{
    sf::Vector2f plane;
};
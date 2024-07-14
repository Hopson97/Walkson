#pragma once

#include <cmath>
#include <iostream>
#include <numbers>

#include <SFML/System/Vector2.hpp>

namespace Maths
{
    // Degrees -> Radian convertion
    template <typename T>
    T rads(T degrees)
    {
        return degrees * (std::numbers::pi_v<T> / 180);
    }

    // 2D Matrix rotation
    template <typename T, typename D>
    sf::Vector2<T> rotate(const sf::Vector2<T>& vec, D degrees)
    {
        D rads = Maths::rads(degrees);

        T nx = vec.x * std::cos(rads) - vec.y * std::sin(rads);
        T ny = vec.x * std::sin(rads) + vec.y * std::cos(rads);

        return {nx, ny};
    }

    // Get the perpendicular of a vector
    template <typename T>
    sf::Vector2<T> perpendicular(const sf::Vector2<T>& vec)
    {
        return {vec.y, -vec.x};
    }

    // Cast a vector internal type from T to N
    template <typename N, typename T>
    sf::Vector2<N> cast_vector(const sf::Vector2<T>& vec)
    {
        return sf::Vector2<N>{static_cast<N>(vec.x), static_cast<N>(vec.y)};
    }

    // Returns either (1, 0), (-1, 0), (0, -1), or (0, 1) depending on the
    // input vector
    template <typename T>
    sf::Vector2<T> round_vect(const sf::Vector2<T>& vect)
    {
        T vx = static_cast<T>(vect.x);
        T vy = static_cast<T>(vect.y);

        T nx = static_cast<T>(std::abs(vx) > std::abs(vy) ? (vx > 0 ? 1 : -1) : 0);
        T ny = static_cast<T>(std::abs(vy) > std::abs(vx) ? (vy > 0 ? 1 : -1) : 0);

        return sf::Vector2<T>{nx, ny};
    }
} // namespace Maths
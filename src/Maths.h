#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <numbers>

namespace Maths
{

    template <typename T>
    T rads(T degrees)
    {
        return degrees * (std::numbers::pi_v<T> / 180);
    }

    // template <typename T>
    // T clamp_angle(T degrees)
    //{
    //     if (degrees < 0)
    //         return degrees + 360;
    //     else if (degrees > 360)
    //         return degrees - 360;
    //     return degrees;
    // }

    template <typename T, typename D>
    sf::Vector2<T> rotate(const sf::Vector2<T> vec, D degrees)
    {
        D rads = Maths::rads(degrees);

        T nx = vec.x * std::cos(rads) - vec.y * std::sin(rads);
        T ny = vec.x * std::sin(rads) + vec.y * std::cos(rads);

        return {nx, ny};
    }
} // namespace Maths
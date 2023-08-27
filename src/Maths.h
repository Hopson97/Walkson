#pragma once

#include <cmath>
#include <numbers>

template <typename T>
T rads(T degrees)
{
    return degrees * (std::numbers::pi_v<T> / 180);
}

template <typename T>
T clamp_angle(T degrees)
{
    if (degrees < 0)
        return degrees + 360;
    else if (degrees > 360)
        return degrees - 360;
    return degrees;
}
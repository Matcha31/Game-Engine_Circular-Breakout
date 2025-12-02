#pragma once
#include <cmath>
#include "vec4.hpp"

// Simple polar coordinates: radius r, angle a (in radians)
struct Polar
{
    float r;  // radius >= 0
    float a;  // angle in radians, will be normalized to [0, 2π)

    Polar() : r(0.0f), a(0.0f) {}
    Polar(float radius, float angle) : r(radius), a(angle) {}

    // Normalize angle to [0, 2π)
    void normalizeAngleInPlace();
    Polar normalizedAngle() const;

    // Conversions with 2D Cartesian (x,y): we ignore z and w
    static Polar fromCartesian(const Vec4& v);
    Vec4 toCartesian() const;
};

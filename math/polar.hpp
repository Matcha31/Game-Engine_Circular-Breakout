#pragma once
#include <cmath>
#include "vec4.hpp"

struct Polar
{
    float r;
    float a;

    Polar() : r(0.0f), a(0.0f) {}
    Polar(float radius, float angle) : r(radius), a(angle) {}

    void normalizeAngleInPlace();
    Polar normalizedAngle() const;

    static Polar fromCartesian(const Vec4& v);
    Vec4 toCartesian() const;
};

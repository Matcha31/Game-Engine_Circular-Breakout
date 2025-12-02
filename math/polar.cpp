#include "polar.hpp"

#include "polar.hpp"
#include <cmath>

namespace
{
    constexpr float TWO_PI = 2.0f * M_PI;

    inline float normalizeAngleTo0_2pi(float angle)
    {
        if (!std::isfinite(angle))
            return 0.0f;

        // Use explicit "number of revolutions" instead of fmod to be more stable
        float revolutions = std::floor(angle / TWO_PI);
        float a = angle - revolutions * TWO_PI;

        if (a < 0.0f)
            a += TWO_PI;

        // Snap very close values to exact 0 to avoid 2π - eps issues
        const float eps = 1e-5f;
        if (std::fabs(a) < eps || std::fabs(a - TWO_PI) < eps)
            a = 0.0f;

        return a;
    }
}

void Polar::normalizeAngleInPlace()
{
    a = normalizeAngleTo0_2pi(a);
}

Polar Polar::normalizedAngle() const
{
    Polar p = *this;
    p.normalizeAngleInPlace();
    return p;
}

Polar Polar::fromCartesian(const Vec4& v)
{
    float x = v.x;
    float y = v.y;

    float radius = std::sqrt(x*x + y*y);
    if (radius == 0.0f)
    {
        return Polar(0.0f, 0.0f);
    }

    float angle = std::atan2(y, x);
    angle = normalizeAngleTo0_2pi(angle);

    return Polar(radius, angle);
}

Vec4 Polar::toCartesian() const
{
    float ang = normalizeAngleTo0_2pi(a);
    float x = r * std::cos(ang);
    float y = r * std::sin(ang);

    return Vec4(x, y, 0.0f, 0.0f);
}

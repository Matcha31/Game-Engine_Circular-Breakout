#include "axis_angle.hpp"
#include "quaternion.hpp"

AxisAngle::AxisAngle()
    : axis(Vec4(1,0,0,0)), angle(0.0f) {}

AxisAngle::AxisAngle(const Vec4& ax, float ang)
    : axis(ax.normalized()), angle(ang) {}

Quaternion AxisAngle::toQuaternion() const
{
    float half = angle * 0.5f;
    float s = std::sin(half);

    return Quaternion(
        axis.x * s,
        axis.y * s,
        axis.z * s,
        std::cos(half)
    );
}

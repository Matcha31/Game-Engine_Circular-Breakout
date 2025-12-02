#pragma once
#include "vec4.hpp"
#include <cmath>

class Quaternion; // forward declaration

class AxisAngle
{
public:
    Vec4 axis;   // should be unit length; w ignored
    float angle; // radians

    AxisAngle();                     
    AxisAngle(const Vec4& axis, float angle);

    // Convert to quaternion
    Quaternion toQuaternion() const;
};

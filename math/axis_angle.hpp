#pragma once
#include "vec4.hpp"
#include <cmath>

class Quaternion;

class AxisAngle
{
public:
    Vec4 axis;
    float angle;

    AxisAngle();                     
    AxisAngle(const Vec4& axis, float angle);

    Quaternion toQuaternion() const;
};

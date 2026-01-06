#pragma once
#include <cmath>
#include "mat4.hpp"
#include "axis_angle.hpp"

class Quaternion
{
public:
    float x, y, z, w;  // w = scalar part

    Quaternion();  
    Quaternion(float x, float y, float z, float w);

    Quaternion conjugate() const;

    float length() const;
    float lengthSquared() const;

    Quaternion normalized() const;
    void normalizeInPlace();

    Quaternion inverse() const;

    static float dot(const Quaternion& a, const Quaternion& b);

    // SLERP
    static Quaternion slerp(
        const Quaternion& a,
        const Quaternion& b,
        float t);

    AxisAngle toAxisAngle() const;

    Mat4 toRotationMatrix() const;
};

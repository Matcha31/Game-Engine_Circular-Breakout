#pragma once
#include <cmath>
#include "vec4.hpp"
#include "axis_angle.hpp"

class Quaternion
{
public:
    float x, y, z, w;  // w = scalar part

    Quaternion();  
    Quaternion(float x, float y, float z, float w);

    // Conjugate: reverse sign of vector part  
    Quaternion conjugate() const;

    float length() const;
    float lengthSquared() const;

    Quaternion normalized() const;
    void normalizeInPlace();

    Quaternion inverse() const;

    // Dot product
    static float dot(const Quaternion& a, const Quaternion& b);

    // SLERP
    static Quaternion slerp(
        const Quaternion& a,
        const Quaternion& b,
        float t);

    // Convert to AxisAngle
    AxisAngle toAxisAngle() const;

    // Optional: to rotation matrix
    Mat4 toRotationMatrix() const;
};

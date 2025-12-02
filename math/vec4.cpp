#include "vec4.hpp"

float Vec4::length() const
{
    // classic Euclidean length
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::lengthSquared() const
{
    // useful to avoid sqrt when comparing
    return x * x + y * y + z * z + w * w;
}

Vec4 Vec4::normalized() const
{
    float len = length();
    if (len == 0.0f)
    {
        // can't normalize zero vector, return copy
        return *this;
    }
    float inv = 1.0f / len;
    return Vec4(x * inv, y * inv, z * inv, w * inv);
}

void Vec4::normalizeInPlace()
{
    float len = length();
    if (len == 0.0f)
        return;
    float inv = 1.0f / len;
    x *= inv;
    y *= inv;
    z *= inv;
    w *= inv;
}

float Vec4::dot(const Vec4& a, const Vec4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Vec4 Vec4::cross3(const Vec4& a, const Vec4& b)
{
    // I'm ignoring w when doing the cross because cross is defined for 3D.
    // This should be fine for directions we'll use for basis later.
    return Vec4(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f // direction
    );
}

std::ostream& operator<<(std::ostream& os, const Vec4& v)
{
    os << "Vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}

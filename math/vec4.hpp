#pragma once

#include <cmath>
#include <ostream>

class Vec4
{
public:
    float x;
    float y;
    float z;
    float w;

    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(float xx, float yy, float zz, float ww)
        : x(xx), y(yy), z(zz), w(ww) {}

    static Vec4 zero() { return Vec4(0.f, 0.f, 0.f, 0.f); }
    static Vec4 unitX() { return Vec4(1.f, 0.f, 0.f, 0.f); }
    static Vec4 unitY() { return Vec4(0.f, 1.f, 0.f, 0.f); }
    static Vec4 unitZ() { return Vec4(0.f, 0.f, 1.f, 0.f); }
    static Vec4 unitW() { return Vec4(0.f, 0.f, 0.f, 1.f); }

    float length() const;
    float lengthSquared() const;

    Vec4 normalized() const;
    void normalizeInPlace();

    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

    Vec4 operator+(const Vec4& rhs) const { return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
    Vec4 operator-(const Vec4& rhs) const { return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }

    Vec4& operator+=(const Vec4& rhs)
    {
        x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
        return *this;
    }

    Vec4& operator-=(const Vec4& rhs)
    {
        x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
        return *this;
    }

    Vec4 operator*(float s) const { return Vec4(x * s, y * s, z * s, w * s); }
    Vec4 operator/(float s) const { return Vec4(x / s, y / s, z / s, w / s); }

    Vec4& operator*=(float s)
    {
        x *= s; y *= s; z *= s; w *= s;
        return *this;
    }

    Vec4& operator/=(float s)
    {
        x /= s; y /= s; z /= s; w /= s;
        return *this;
    }

    static float dot(const Vec4& a, const Vec4& b);

    static Vec4 cross3(const Vec4& a, const Vec4& b);

    friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
};

inline Vec4 operator*(float s, const Vec4& v)
{
    return v * s;
}

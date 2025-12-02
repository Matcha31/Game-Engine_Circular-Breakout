#pragma once

#include <cmath>
#include <ostream>

// I'll keep this simple for now, later I can wrap it in a namespace if needed.
class Vec4
{
public:
    float x;
    float y;
    float z;
    float w;

    // default: zero vector
    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    // explicit constructor
    Vec4(float xx, float yy, float zz, float ww)
        : x(xx), y(yy), z(zz), w(ww) {}

    // handy presets
    static Vec4 zero() { return Vec4(0.f, 0.f, 0.f, 0.f); }
    static Vec4 unitX() { return Vec4(1.f, 0.f, 0.f, 0.f); }
    static Vec4 unitY() { return Vec4(0.f, 1.f, 0.f, 0.f); }
    static Vec4 unitZ() { return Vec4(0.f, 0.f, 1.f, 0.f); }
    // w=1.0f in case I want a "point" later for matrices
    static Vec4 unitW() { return Vec4(0.f, 0.f, 0.f, 1.f); }

    // length and normalization
    float length() const;
    float lengthSquared() const;

    // returns a normalized copy, doesn't touch this
    Vec4 normalized() const;
    // in-place normalize (I'll just skip if length is 0)
    void normalizeInPlace();

    // opposite vector
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

    // add/sub
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

    // scalar mul/div
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

    // dot product (the assignment wants this)
    static float dot(const Vec4& a, const Vec4& b);

    // cross product: assignment wants it, but cross is really 3D.
    // I'll do it on (x,y,z) and set w=0 so it behaves like a direction.
    static Vec4 cross3(const Vec4& a, const Vec4& b);

    // small helper for printing during tests
    friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
};

// allow scalar * vector
inline Vec4 operator*(float s, const Vec4& v)
{
    return v * s;
}

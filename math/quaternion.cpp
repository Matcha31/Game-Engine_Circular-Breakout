#include "quaternion.hpp"
#include "mat4.hpp"

Quaternion::Quaternion()
    : x(0), y(0), z(0), w(1) {}

Quaternion::Quaternion(float xx, float yy, float zz, float ww)
    : x(xx), y(yy), z(zz), w(ww) {}

Quaternion Quaternion::conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

float Quaternion::length() const
{
    return std::sqrt(x*x + y*y + z*z + w*w);
}

float Quaternion::lengthSquared() const
{
    return x*x + y*y + z*z + w*w;
}

Quaternion Quaternion::normalized() const
{
    float len = length();
    if (len == 0) return *this;
    float inv = 1.0f / len;
    return Quaternion(x*inv, y*inv, z*inv, w*inv);
}

void Quaternion::normalizeInPlace()
{
    float len = length();
    if (len == 0) return;
    float inv = 1.0f / len;

    x *= inv;
    y *= inv;
    z *= inv;
    w *= inv;
}

Quaternion Quaternion::inverse() const
{
    float ls = lengthSquared();
    if (ls == 0) return *this;

    Quaternion c = conjugate();
    return Quaternion(c.x / ls, c.y / ls, c.z / ls, c.w / ls);
}

float Quaternion::dot(const Quaternion& a, const Quaternion& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b, float t)
{
    Quaternion q1 = a.normalized();
    Quaternion q2 = b.normalized();

    float d = dot(q1, q2);

    if (d < 0.0f) {
        d = -d;
        q2 = Quaternion(-q2.x, -q2.y, -q2.z, -q2.w);
    }

    if (d > 0.9995f) {
        Quaternion r(
            q1.x + t*(q2.x - q1.x),
            q1.y + t*(q2.y - q1.y),
            q1.z + t*(q2.z - q1.z),
            q1.w + t*(q2.w - q1.w)
        );
        return r.normalized();
    }

    float theta = std::acos(d);
    float s = std::sin(theta);

    float w1 = std::sin((1.0f - t) * theta) / s;
    float w2 = std::sin(t * theta) / s;

    return Quaternion(
        q1.x * w1 + q2.x * w2,
        q1.y * w1 + q2.y * w2,
        q1.z * w1 + q2.z * w2,
        q1.w * w1 + q2.w * w2
    );
}

AxisAngle Quaternion::toAxisAngle() const
{
    Quaternion q = normalized();

    float angle = 2.0f * std::acos(q.w);
    float s = std::sqrt(1 - q.w*q.w);

    if (s < 1e-6f) {
        return AxisAngle(Vec4(1,0,0,0), 0);
    }

    return AxisAngle(
        Vec4(q.x/s, q.y/s, q.z/s, 0),
        angle
    );
}

Mat4 Quaternion::toRotationMatrix() const
{
    Quaternion q = normalized();

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return Mat4({
        1 - 2*(yy + zz),  2*(xy + wz),      2*(xz - wy),      0,
        2*(xy - wz),      1 - 2*(xx + zz),  2*(yz + wx),      0,
        2*(xz + wy),      2*(yz - wx),      1 - 2*(xx + yy),  0,
        0,                0,                0,                1
    });
}

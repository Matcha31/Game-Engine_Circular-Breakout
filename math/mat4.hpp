#pragma once
#include "vec4.hpp"
#include <initializer_list>
#include <cstring>

class Mat4 {
public:
    float m[16]; // column-major order (OpenGL convention)

    Mat4();
    Mat4(float diagonal);
    Mat4(const std::initializer_list<float>& list);

    static Mat4 identity();
    static Mat4 zeros();

    Mat4 transpose() const;
    Mat4 operator*(const Mat4& rhs) const;
    Vec4 operator*(const Vec4& v) const;

    float& operator()(int row, int col);
    float  operator()(int row, int col) const;

    static Mat4 translation(float tx, float ty, float tz);
    static Mat4 translation(const Vec4& t);
};

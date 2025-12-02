#pragma once
#include "vec4.hpp"
#include <initializer_list>
#include <cstring>

class Mat4 {
public:
    float m[16]; // column-major order (OpenGL convention)

    //  Constructors 
    Mat4();                         // identity
    Mat4(float diagonal);           // diagonal matrix
    Mat4(const std::initializer_list<float>& list); // explicitly set elements

    //  Static constructors 
    static Mat4 identity();
    static Mat4 zeros();

    //  Basic operations 
    Mat4 transpose() const;
    Mat4 operator*(const Mat4& rhs) const;
    Vec4 operator*(const Vec4& v) const;

    //  Indexing (row, col) 
    float& operator()(int row, int col);
    float  operator()(int row, int col) const;

};

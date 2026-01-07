#include "mat4.hpp"

Mat4::Mat4() {
    for (int i = 0; i < 16; ++i) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

Mat4::Mat4(float diagonal) {
    for (int i = 0; i < 16; ++i) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = diagonal;
}

Mat4::Mat4(const std::initializer_list<float>& list) {
    int count = (int)list.size();
    int n = (count < 16) ? count : 16;

    int i = 0;
    for (float v : list) {
        if (i >= n) break;
        m[i++] = v;
    }
    for (; i < 16; ++i) m[i] = 0.0f;
}

Mat4 Mat4::identity() {
    return Mat4();
}

Mat4 Mat4::zeros() {
    Mat4 r(0.0f); // diagonal = 0
    for (int i = 0; i < 16; i++) r.m[i] = 0.0f;
    return r;
}

float& Mat4::operator()(int row, int col) {
    return m[col * 4 + row];
}

float Mat4::operator()(int row, int col) const {
    return m[col * 4 + row];
}

Mat4 Mat4::transpose() const {
    Mat4 t(0.0f);
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            t(row, col) = (*this)(col, row);
        }
    }
    return t;
}

Mat4 Mat4::operator*(const Mat4& rhs) const {
    Mat4 result(0.0f);

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {

            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += (*this)(row, k) * rhs(k, col);
            }

            result(row, col) = sum;
        }
    }

    return result;
}

Vec4 Mat4::operator*(const Vec4& v) const {
    return Vec4(
        m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
        m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
        m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
        m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
    );
}

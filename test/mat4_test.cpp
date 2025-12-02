#include <gtest/gtest.h>
#include "../math/mat4.hpp"
#include "../math/vec4.hpp"

TEST(Mat4, DefaultConstructorIsIdentity)
{
    Mat4 m;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (r == c) EXPECT_FLOAT_EQ(m(r,c), 1.f);
            else        EXPECT_FLOAT_EQ(m(r,c), 0.f);
}

TEST(Mat4, DiagonalConstructor)
{
    Mat4 m(5.0f);

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (r == c) EXPECT_FLOAT_EQ(m(r,c), 5.f);
            else        EXPECT_FLOAT_EQ(m(r,c), 0.f);
}

TEST(Mat4, InitializerListFull)
{
    Mat4 m({
        1,  2,  3,  4,
        5,  6,  7,  8,
        9, 10, 11, 12,
        13,14, 15, 16
    });

    for (int i = 0; i < 16; ++i)
        EXPECT_FLOAT_EQ(m.m[i], float(i+1));
}

TEST(Mat4, InitializerListPartialZeroFilled)
{
    Mat4 m({
        1, 2, 3, 4,
        5
    });

    EXPECT_FLOAT_EQ(m.m[0], 1);
    EXPECT_FLOAT_EQ(m.m[1], 2);
    EXPECT_FLOAT_EQ(m.m[2], 3);
    EXPECT_FLOAT_EQ(m.m[3], 4);
    EXPECT_FLOAT_EQ(m.m[4], 5);

    for (int i = 5; i < 16; ++i)
        EXPECT_FLOAT_EQ(m.m[i], 0);
}

TEST(Mat4, ZerosStatic)
{
    Mat4 m = Mat4::zeros();

    for (int i = 0; i < 16; ++i)
        EXPECT_FLOAT_EQ(m.m[i], 0.f);
}

TEST(Mat4, IndexingColumnMajor)
{
    Mat4 m({
        1,2,3,4,
        10,20,30,40,
        100,200,300,400,
        7,8,9,10
    });

    EXPECT_FLOAT_EQ(m(0,0), 1);
    EXPECT_FLOAT_EQ(m(1,0), 2);
    EXPECT_FLOAT_EQ(m(2,0), 3);
    EXPECT_FLOAT_EQ(m(3,0), 4);

    EXPECT_FLOAT_EQ(m(0,1), 10);
    EXPECT_FLOAT_EQ(m(1,1), 20);
    EXPECT_FLOAT_EQ(m(2,1), 30);
    EXPECT_FLOAT_EQ(m(3,1), 40);

    EXPECT_FLOAT_EQ(m(0,2), 100);
    EXPECT_FLOAT_EQ(m(1,2), 200);
    EXPECT_FLOAT_EQ(m(2,2), 300);
    EXPECT_FLOAT_EQ(m(3,2), 400);

    EXPECT_FLOAT_EQ(m(0,3), 7);
    EXPECT_FLOAT_EQ(m(1,3), 8);
    EXPECT_FLOAT_EQ(m(2,3), 9);
    EXPECT_FLOAT_EQ(m(3,3), 10);
}

TEST(Mat4, Transpose)
{
    Mat4 m({
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,16
    });

    Mat4 t = m.transpose();

    EXPECT_FLOAT_EQ(t(0,0), 1);
    EXPECT_FLOAT_EQ(t(0,1), 2);
    EXPECT_FLOAT_EQ(t(0,2), 3);
    EXPECT_FLOAT_EQ(t(0,3), 4);

    EXPECT_FLOAT_EQ(t(1,0), 5);
    EXPECT_FLOAT_EQ(t(1,1), 6);
    EXPECT_FLOAT_EQ(t(1,2), 7);
    EXPECT_FLOAT_EQ(t(1,3), 8);

    EXPECT_FLOAT_EQ(t(2,0), 9);
    EXPECT_FLOAT_EQ(t(2,1),10);
    EXPECT_FLOAT_EQ(t(2,2),11);
    EXPECT_FLOAT_EQ(t(2,3),12);

    EXPECT_FLOAT_EQ(t(3,0),13);
    EXPECT_FLOAT_EQ(t(3,1),14);
    EXPECT_FLOAT_EQ(t(3,2),15);
    EXPECT_FLOAT_EQ(t(3,3),16);
}

TEST(Mat4, TransposeSymmetricMatrix)
{
    Mat4 m({
        1, 2, 3, 4,
        2, 6, 7, 8,
        3, 7,11,12,
        4, 8,12,16
    });

    Mat4 t = m.transpose();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT_FLOAT_EQ(t(r,c), m(r,c));
}

TEST(Mat4, MultiplyByIdentity)
{
    Mat4 A({
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,16
    });

    Mat4 I = Mat4::identity();
    Mat4 R = A * I;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT_FLOAT_EQ(R(r,c), A(r,c));
}

TEST(Mat4, IdentityTimesMatrix)
{
    Mat4 A({
        4,3,2,1,
        8,7,6,5,
        12,11,10,9,
        16,15,14,13
    });

    Mat4 I = Mat4::identity();
    Mat4 R = I * A;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT_FLOAT_EQ(R(r,c), A(r,c));
}

TEST(Mat4, MultiplyDiagonal)
{
    Mat4 A(2.f);
    Mat4 B(3.f);

    Mat4 C = A * B;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (r == c) EXPECT_FLOAT_EQ(C(r,c), 6.f);
            else        EXPECT_FLOAT_EQ(C(r,c), 0.f);
}

TEST(Mat4, MultiplyGeneral)
{
    Mat4 A({
        1,2,3,4,
        4,3,2,1,
        1,0,1,0,
        0,1,0,1
    });

    Mat4 B({
        2,1,0,1,
        0,1,2,1,
        1,0,1,0,
        3,3,3,3
    });

    Mat4 C = A * B;

    auto expected = [&](int r, int c) {
        float sum = 0.f;
        for (int k = 0; k < 4; ++k)
            sum += A(r,k) * B(k,c);
        return sum;
    };

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT_FLOAT_EQ(C(r,c), expected(r,c));
}

TEST(Mat4, AssociativityApprox)
{
    Mat4 A({
        1,3,5,7,
        2,4,6,8,
        1,0,1,0,
        0,1,0,1
    });

    Mat4 B({
        4,3,2,1,
        1,2,3,4,
        5,6,7,8,
        1,1,1,1
    });

    Mat4 C({
        0,1,0,1,
        1,0,1,0,
        2,2,2,2,
        3,3,3,3
    });

    Mat4 left  = A * (B * C);
    Mat4 right = (A * B) * C;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT_NEAR(left(r,c), right(r,c), 1e-5f);
}

TEST(Mat4, MultiplyVec4Identity)
{
    Mat4 I = Mat4::identity();
    Vec4 v(1,2,3,4);

    Vec4 r = I * v;

    EXPECT_FLOAT_EQ(r.x, 1);
    EXPECT_FLOAT_EQ(r.y, 2);
    EXPECT_FLOAT_EQ(r.z, 3);
    EXPECT_FLOAT_EQ(r.w, 4);
}

TEST(Mat4, MultiplyVec4General)
{
    Mat4 M({
        1,0,0,1,
        0,1,0,2,
        0,0,1,3,
        0,0,0,1
    });

    Vec4 v(1,2,3,1);
    Vec4 r = M * v;

    auto expected = [&](int row) {
        return M(row,0)*v.x + M(row,1)*v.y + M(row,2)*v.z + M(row,3)*v.w;
    };

    EXPECT_FLOAT_EQ(r.x, expected(0));
    EXPECT_FLOAT_EQ(r.y, expected(1));
    EXPECT_FLOAT_EQ(r.z, expected(2));
    EXPECT_FLOAT_EQ(r.w, expected(3));
}

TEST(Mat4, MultiplyVec4BasisVectors)
{
    Mat4 M({
        2,0,0,0,
        0,3,0,0,
        0,0,4,0,
        0,0,0,5
    });

    Vec4 eX(1,0,0,0);
    Vec4 eY(0,1,0,0);
    Vec4 eZ(0,0,1,0);
    Vec4 eW(0,0,0,1);

    Vec4 rx = M * eX;
    Vec4 ry = M * eY;
    Vec4 rz = M * eZ;
    Vec4 rw = M * eW;

    EXPECT_FLOAT_EQ(rx.x, 2);
    EXPECT_FLOAT_EQ(ry.y, 3);
    EXPECT_FLOAT_EQ(rz.z, 4);
    EXPECT_FLOAT_EQ(rw.w, 5);
}

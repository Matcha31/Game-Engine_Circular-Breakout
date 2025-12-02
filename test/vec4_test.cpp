#include "../math/vec4.hpp"
#include <gtest/gtest.h>

TEST(Vec4, DefaultConstructorZero)
{
    Vec4 v;
    EXPECT_FLOAT_EQ(v.x, 0.f);
    EXPECT_FLOAT_EQ(v.y, 0.f);
    EXPECT_FLOAT_EQ(v.z, 0.f);
    EXPECT_FLOAT_EQ(v.w, 0.f);
}

TEST(Vec4, ExplicitConstructor)
{
    Vec4 v(1.f, 2.f, 3.f, 4.f);
    EXPECT_FLOAT_EQ(v.x, 1.f);
    EXPECT_FLOAT_EQ(v.y, 2.f);
    EXPECT_FLOAT_EQ(v.z, 3.f);
    EXPECT_FLOAT_EQ(v.w, 4.f);
}

TEST(Vec4, Presets)
{
    Vec4 z = Vec4::zero();
    EXPECT_FLOAT_EQ(z.x, 0.f);
    EXPECT_FLOAT_EQ(z.y, 0.f);
    EXPECT_FLOAT_EQ(z.z, 0.f);
    EXPECT_FLOAT_EQ(z.w, 0.f);

    Vec4 ex = Vec4::unitX();
    EXPECT_FLOAT_EQ(ex.x, 1.f);
    EXPECT_FLOAT_EQ(ex.y, 0.f);
    EXPECT_FLOAT_EQ(ex.z, 0.f);
    EXPECT_FLOAT_EQ(ex.w, 0.f);

    Vec4 ey = Vec4::unitY();
    EXPECT_FLOAT_EQ(ey.x, 0.f);
    EXPECT_FLOAT_EQ(ey.y, 1.f);
    EXPECT_FLOAT_EQ(ey.z, 0.f);
    EXPECT_FLOAT_EQ(ey.w, 0.f);

    Vec4 ez = Vec4::unitZ();
    EXPECT_FLOAT_EQ(ez.x, 0.f);
    EXPECT_FLOAT_EQ(ez.y, 0.f);
    EXPECT_FLOAT_EQ(ez.z, 1.f);
    EXPECT_FLOAT_EQ(ez.w, 0.f);

    Vec4 ew = Vec4::unitW();
    EXPECT_FLOAT_EQ(ew.x, 0.f);
    EXPECT_FLOAT_EQ(ew.y, 0.f);
    EXPECT_FLOAT_EQ(ew.z, 0.f);
    EXPECT_FLOAT_EQ(ew.w, 1.f);
}

TEST(Vec4, LengthSquared)
{
    Vec4 v(1.f, 2.f, 3.f, 4.f);
    EXPECT_FLOAT_EQ(v.lengthSquared(), 1 + 4 + 9 + 16); // = 30
}

TEST(Vec4, Length)
{
    Vec4 v(0.f, 3.f, 4.f, 0.f); // length = 5
    EXPECT_FLOAT_EQ(v.length(), 5.f);
}

// Normalization

TEST(Vec4, Normalized)
{
    Vec4 v(0.f, 3.f, 4.f, 0.f);
    Vec4 n = v.normalized();

    EXPECT_NEAR(n.x, 0.f, 1e-6);
    EXPECT_NEAR(n.y, 0.6f, 1e-6);
    EXPECT_NEAR(n.z, 0.8f, 1e-6);
    EXPECT_NEAR(n.w, 0.f, 1e-6);
}

TEST(Vec4, NormalizeInPlace)
{
    Vec4 v(0.f, 3.f, 4.f, 0.f);
    v.normalizeInPlace();

    EXPECT_NEAR(v.x, 0.f, 1e-6);
    EXPECT_NEAR(v.y, 0.6f, 1e-6);
    EXPECT_NEAR(v.z, 0.8f, 1e-6);
    EXPECT_NEAR(v.w, 0.f, 1e-6);
}

TEST(Vec4, NormalizeZeroVectorDoesNothing)
{
    Vec4 v(0.f, 0.f, 0.f, 0.f);
    v.normalizeInPlace();

    EXPECT_FLOAT_EQ(v.x, 0.f);
    EXPECT_FLOAT_EQ(v.y, 0.f);
    EXPECT_FLOAT_EQ(v.z, 0.f);
    EXPECT_FLOAT_EQ(v.w, 0.f);
}

TEST(Vec4, AddOperator)
{
    Vec4 a(1, 2, 3, 4);
    Vec4 b(10, 20, 30, 40);

    Vec4 r = a + b;
    EXPECT_FLOAT_EQ(r.x, 11);
    EXPECT_FLOAT_EQ(r.y, 22);
    EXPECT_FLOAT_EQ(r.z, 33);
    EXPECT_FLOAT_EQ(r.w, 44);
}

TEST(Vec4, SubOperator)
{
    Vec4 a(10, 20, 30, 40);
    Vec4 b(1, 2, 3, 4);

    Vec4 r = a - b;
    EXPECT_FLOAT_EQ(r.x, 9);
    EXPECT_FLOAT_EQ(r.y, 18);
    EXPECT_FLOAT_EQ(r.z, 27);
    EXPECT_FLOAT_EQ(r.w, 36);
}

TEST(Vec4, AddAssign)
{
    Vec4 a(1, 1, 1, 1);
    Vec4 b(2, 3, 4, 5);

    a += b;

    EXPECT_FLOAT_EQ(a.x, 3);
    EXPECT_FLOAT_EQ(a.y, 4);
    EXPECT_FLOAT_EQ(a.z, 5);
    EXPECT_FLOAT_EQ(a.w, 6);
}

TEST(Vec4, SubAssign)
{
    Vec4 a(10, 10, 10, 10);
    Vec4 b(1, 2, 3, 4);

    a -= b;

    EXPECT_FLOAT_EQ(a.x, 9);
    EXPECT_FLOAT_EQ(a.y, 8);
    EXPECT_FLOAT_EQ(a.z, 7);
    EXPECT_FLOAT_EQ(a.w, 6);
}

TEST(Vec4, ScalarMul)
{
    Vec4 v(1, 2, 3, 4);
    Vec4 r = v * 2.f;

    EXPECT_FLOAT_EQ(r.x, 2);
    EXPECT_FLOAT_EQ(r.y, 4);
    EXPECT_FLOAT_EQ(r.z, 6);
    EXPECT_FLOAT_EQ(r.w, 8);
}

TEST(Vec4, ScalarMulReverse)
{
    Vec4 v(1, 2, 3, 4);
    Vec4 r = 3.f * v; // uses global operator*

    EXPECT_FLOAT_EQ(r.x, 3);
    EXPECT_FLOAT_EQ(r.y, 6);
    EXPECT_FLOAT_EQ(r.z, 9);
    EXPECT_FLOAT_EQ(r.w, 12);
}

TEST(Vec4, ScalarDiv)
{
    Vec4 v(2, 4, 6, 8);
    Vec4 r = v / 2.f;

    EXPECT_FLOAT_EQ(r.x, 1);
    EXPECT_FLOAT_EQ(r.y, 2);
    EXPECT_FLOAT_EQ(r.z, 3);
    EXPECT_FLOAT_EQ(r.w, 4);
}

TEST(Vec4, ScalarMulAssign)
{
    Vec4 v(1, 2, 3, 4);
    v *= 3.f;

    EXPECT_FLOAT_EQ(v.x, 3);
    EXPECT_FLOAT_EQ(v.y, 6);
    EXPECT_FLOAT_EQ(v.z, 9);
    EXPECT_FLOAT_EQ(v.w, 12);
}

TEST(Vec4, ScalarDivAssign)
{
    Vec4 v(2, 4, 6, 8);
    v /= 2.f;

    EXPECT_FLOAT_EQ(v.x, 1);
    EXPECT_FLOAT_EQ(v.y, 2);
    EXPECT_FLOAT_EQ(v.z, 3);
    EXPECT_FLOAT_EQ(v.w, 4);
}

TEST(Vec4, UnaryMinus)
{
    Vec4 v(1, -2, 3, -4);
    Vec4 r = -v;

    EXPECT_FLOAT_EQ(r.x, -1);
    EXPECT_FLOAT_EQ(r.y, 2);
    EXPECT_FLOAT_EQ(r.z, -3);
    EXPECT_FLOAT_EQ(r.w, 4);
}

TEST(Vec4, DotProduct)
{
    Vec4 a(1, 2, 3, 4);
    Vec4 b(-1, 5, 2, 0);

    float d = Vec4::dot(a, b);
    EXPECT_FLOAT_EQ(d, 1*(-1) + 2*5 + 3*2 + 4*0);
}

TEST(Vec4, Cross3)
{
    Vec4 a(1, 0, 0, 7);  // w ignored
    Vec4 b(0, 1, 0, 5);

    Vec4 r = Vec4::cross3(a, b);

    EXPECT_FLOAT_EQ(r.x, 0);
    EXPECT_FLOAT_EQ(r.y, 0);
    EXPECT_FLOAT_EQ(r.z, 1);
    EXPECT_FLOAT_EQ(r.w, 0);  // must always be 0
}

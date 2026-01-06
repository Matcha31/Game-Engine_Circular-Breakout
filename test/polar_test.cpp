#include <gtest/gtest.h>
#include "../math/polar.hpp"

// For comparison with "nice" angles
namespace {
    constexpr float TWO_PI = 2.0f * M_PI;
}

TEST(Polar, NormalizeAngleInPlace_PositiveWrap)
{
    Polar p(1.0f, 3.0f * TWO_PI); // 3 * 2π
    p.normalizeAngleInPlace();

    EXPECT_NEAR(p.a, 0.0f, 1e-6f);
}

TEST(Polar, NormalizeAngleInPlace_NegativeAngle)
{
    Polar p(1.0f, -M_PI / 2.0f); // -90°
    p.normalizeAngleInPlace();

    EXPECT_NEAR(p.a, 1.5f * M_PI, 1e-6f); // 3π/2
}

TEST(Polar, NormalizedAngle_DoesNotModifyOriginal)
{
    Polar p(1.0f, -M_PI);
    Polar n = p.normalizedAngle();

    EXPECT_FLOAT_EQ(p.a, -M_PI);             // original unchanged
    EXPECT_NEAR(n.a, M_PI, 1e-6f);           // -π → π
}

TEST(Polar, FromCartesian_UnitX)
{
    Vec4 v(1.0f, 0.0f, 0.0f, 0.0f);
    Polar p = Polar::fromCartesian(v);

    EXPECT_NEAR(p.r, 1.0f, 1e-6f);
    EXPECT_NEAR(p.a, 0.0f, 1e-6f);
}

TEST(Polar, FromCartesian_UnitY)
{
    Vec4 v(0.0f, 1.0f, 0.0f, 0.0f);
    Polar p = Polar::fromCartesian(v);

    EXPECT_NEAR(p.r, 1.0f, 1e-6f);
    EXPECT_NEAR(p.a, M_PI / 2.0f, 1e-6f);
}

TEST(Polar, FromCartesian_NegativeX)
{
    Vec4 v(-1.0f, 0.0f, 0.0f, 0.0f);
    Polar p = Polar::fromCartesian(v);

    EXPECT_NEAR(p.r, 1.0f, 1e-6f);
    EXPECT_NEAR(p.a, M_PI, 1e-6f);
}

TEST(Polar, FromCartesian_NegativeY)
{
    Vec4 v(0.0f, -1.0f, 0.0f, 0.0f);
    Polar p = Polar::fromCartesian(v);

    EXPECT_NEAR(p.r, 1.0f, 1e-6f);
    EXPECT_NEAR(p.a, 1.5f * M_PI, 1e-6f); // 3π/2
}

TEST(Polar, FromCartesian_ZeroVector)
{
    Vec4 v(0.0f, 0.0f, 0.0f, 0.0f);
    Polar p = Polar::fromCartesian(v);

    EXPECT_NEAR(p.r, 0.0f, 1e-6f);
    EXPECT_NEAR(p.a, 0.0f, 1e-6f); // defined convention
}

TEST(Polar, ToCartesian_AxisAligned)
{
    Polar p1(1.0f, 0.0f);        // along +X
    Polar p2(1.0f, M_PI / 2.0f);   // along +Y

    Vec4 v1 = p1.toCartesian();
    Vec4 v2 = p2.toCartesian();

    EXPECT_NEAR(v1.x, 1.0f, 1e-6f);
    EXPECT_NEAR(v1.y, 0.0f, 1e-6f);
    EXPECT_NEAR(v1.z, 0.0f, 1e-6f);

    EXPECT_NEAR(v2.x, 0.0f, 1e-6f);
    EXPECT_NEAR(v2.y, 1.0f, 1e-6f);
    EXPECT_NEAR(v2.z, 0.0f, 1e-6f);
}

TEST(Polar, RoundTrip_CartesianPolarCartesian)
{
    Vec4 v(2.0f, -2.0f, 5.0f, 1.0f); // z,w ignored
    Polar p = Polar::fromCartesian(v);
    Vec4 v2 = p.toCartesian();

    float expectedR = std::sqrt(8.0f);

    EXPECT_NEAR(p.r, expectedR, 1e-6f);

    EXPECT_NEAR(v2.x, v.x, 1e-5f);
    EXPECT_NEAR(v2.y, v.y, 1e-5f);
    EXPECT_NEAR(v2.z, 0.0f, 1e-6f);
}

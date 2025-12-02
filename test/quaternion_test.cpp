#include <gtest/gtest.h>
#include <cmath>
#include "../math/quaternion.hpp"

TEST(Quaternion, Conjugate)
{
    Quaternion q(1,2,3,4);
    Quaternion c = q.conjugate();

    EXPECT_FLOAT_EQ(c.x, -1);
    EXPECT_FLOAT_EQ(c.y, -2);
    EXPECT_FLOAT_EQ(c.z, -3);
    EXPECT_FLOAT_EQ(c.w, 4);
}

TEST(Quaternion, Length)
{
    Quaternion q(1,2,2,1);
    EXPECT_FLOAT_EQ(q.length(), std::sqrt(1*1 + 2*2 + 2*2 + 1*1));
}

TEST(Quaternion, Normalize)
{
    Quaternion q(0,3,4,0);
    Quaternion n = q.normalized();

    float len = std::sqrt(3.0f*3.0f + 4.0f*4.0f);

    EXPECT_NEAR(n.x, 0.0f, 1e-6f);
    EXPECT_NEAR(n.y, 3.0f/len, 1e-6f);
    EXPECT_NEAR(n.z, 4.0f/len, 1e-6f);
    EXPECT_NEAR(n.w, 0.0f, 1e-6f);
}

TEST(Quaternion, NormalizeInPlace)
{
    Quaternion q(0,3,4,0);
    q.normalizeInPlace();

    float len = std::sqrt(3.0f*3.0f + 4.0f*4.0f);

    EXPECT_NEAR(q.x, 0.0f, 1e-6f);
    EXPECT_NEAR(q.y, 3.0f/len, 1e-6f);
    EXPECT_NEAR(q.z, 4.0f/len, 1e-6f);
    EXPECT_NEAR(q.w, 0.0f, 1e-6f);
}

TEST(Quaternion, Inverse)
{
    Quaternion q(0.3f, -0.5f, 0.7f, 0.2f);

    Quaternion inv = q.inverse();
    Quaternion conj = q.conjugate();
    float ls = q.lengthSquared();

    EXPECT_NEAR(inv.x, conj.x / ls, 1e-6f);
    EXPECT_NEAR(inv.y, conj.y / ls, 1e-6f);
    EXPECT_NEAR(inv.z, conj.z / ls, 1e-6f);
    EXPECT_NEAR(inv.w, conj.w / ls, 1e-6f);
}

TEST(Quaternion, Dot)
{
    Quaternion a(1,2,3,4);
    Quaternion b(2,1,0,1);

    EXPECT_FLOAT_EQ(Quaternion::dot(a,b),
                    1*2 + 2*1 + 3*0 + 4*1);
}

TEST(Quaternion, ToAxisAngle)
{
    const float angle = static_cast<float>(M_PI) / 2.0f;
    const float s = std::sin(angle * 0.5f);
    const float c = std::cos(angle * 0.5f);

    Quaternion q(0.0f, s, 0.0f, c);

    AxisAngle aa = q.toAxisAngle();

    EXPECT_NEAR(aa.axis.x, 0.0f, 1e-5f);
    EXPECT_NEAR(aa.axis.y, 1.0f, 1e-5f);
    EXPECT_NEAR(aa.axis.z, 0.0f, 1e-5f);

    EXPECT_NEAR(aa.angle, angle, 1e-5f);
}

TEST(Quaternion, SLERP_Halfway)
{
    Quaternion q1(0,0,0,1);
    Quaternion q2(0,1,0,0);

    Quaternion h = Quaternion::slerp(q1, q2, 0.5f);
    AxisAngle aa = h.toAxisAngle();

    EXPECT_NEAR(aa.angle, static_cast<float>(M_PI) / 2.0f, 1e-5f);
    EXPECT_NEAR(aa.axis.x, 0.0f, 1e-5f);
    EXPECT_NEAR(aa.axis.y, 1.0f, 1e-5f);
    EXPECT_NEAR(aa.axis.z, 0.0f, 1e-5f);
}

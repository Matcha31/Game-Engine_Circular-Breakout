#include <gtest/gtest.h>
#include "../math/axis_angle.hpp"
#include "../math/quaternion.hpp"

TEST(AxisAngle, ToQuaternion)
{
    AxisAngle aa(Vec4(0,1,0,0), M_PI / 2);

    Quaternion q = aa.toQuaternion();

    EXPECT_NEAR(q.x, 0, 1e-6);
    EXPECT_NEAR(q.y, std::sin(M_PI/4), 1e-6);
    EXPECT_NEAR(q.z, 0, 1e-6);
    EXPECT_NEAR(q.w, std::cos(M_PI/4), 1e-6);
}

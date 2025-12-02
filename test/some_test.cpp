// This is a tutorial file. Feel free to remove it.

#include "../some_my_library_dir/some_library_file.hpp"
#include <gtest/gtest.h>

TEST(some_test_suite, some_test)
{
    EXPECT_EQ(some_lib::some_my_library_function(1), 2) << "2*1==2.";
}

TEST(some_test_suite, another_test)
{
    EXPECT_EQ(some_lib::some_my_library_function(3), 6) << "2*3==6.";
}

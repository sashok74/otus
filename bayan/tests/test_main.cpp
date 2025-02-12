#include <gtest/gtest.h>

#include "finddub/finddub.h"

TEST(VersionTest, ReturnsPositiveValue) { EXPECT_GT(version(), 0); }

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
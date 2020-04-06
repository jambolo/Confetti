#include "Confetti/Configuration.h"
#include "gtest/gtest.h"

using namespace Confetti;

TEST(ConfigurationTest, Constructor_default)
{
    Configuration c;
    EXPECT_EQ(c.emitters_.size(), 0);
    EXPECT_EQ(c.emitterVolumes_.size(), 0);
    EXPECT_EQ(c.environments_.size(), 0);
    EXPECT_EQ(c.appearances_.size(), 0);
    EXPECT_EQ(c.clipperLists_.size(), 0);
    EXPECT_EQ(c.surfaceLists_.size(), 0);
}

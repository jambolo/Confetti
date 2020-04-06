#include "Confetti/JsonConfiguration.h"
#include "gtest/gtest.h"

using namespace Confetti;
using namespace nlohmann;

TEST(JsonConfigurationTest, Constructor_json)
{
    ASSERT_NO_THROW({ json j; JsonConfiguration c(j); });

    {
        json j;
        JsonConfiguration c(j);
        EXPECT_EQ(c.emitters_.size(), 0);
        EXPECT_EQ(c.emitterVolumes_.size(), 0);
        EXPECT_EQ(c.environments_.size(), 0);
        EXPECT_EQ(c.appearances_.size(), 0);
        EXPECT_EQ(c.clipperLists_.size(), 0);
        EXPECT_EQ(c.surfaceLists_.size(), 0);
    }

    ASSERT_NO_THROW({ json j = json::parse("{}"); JsonConfiguration c(j); });

    {
        json j = json::parse("{}");
        JsonConfiguration c(j);
        EXPECT_EQ(c.emitters_.size(), 0);
        EXPECT_EQ(c.emitterVolumes_.size(), 0);
        EXPECT_EQ(c.environments_.size(), 0);
        EXPECT_EQ(c.appearances_.size(), 0);
        EXPECT_EQ(c.clipperLists_.size(), 0);
        EXPECT_EQ(c.surfaceLists_.size(), 0);
    }
}

TEST(JsonConfigurationTest, Constructor_path)
{
    EXPECT_THROW(JsonConfiguration c("non-existent"), std::runtime_error);
    EXPECT_NO_THROW(JsonConfiguration c("test-JsonConfiguration.json"));
}

TEST(JsonConfigurationTest, DISABLED_toJson)
{
    GTEST_SKIP();
}

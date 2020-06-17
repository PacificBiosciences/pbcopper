#include <pbcopper/utility/OStreamRedirect.h>

#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

TEST(Utility_OStreamRedirect, can_redirect_stdout)
{
    std::ostringstream stream;
    PacBio::Utility::CoutRedirect redirect{stream.rdbuf()};

    const std::string text{"Hello redirect."};
    std::cout << text;
    EXPECT_EQ(text, stream.str());
}

TEST(Utility_OStreamRedirect, can_redirect_stderr)
{
    std::ostringstream stream;
    PacBio::Utility::CerrRedirect redirect{stream.rdbuf()};

    const std::string text{"Hello redirect."};
    std::cerr << text;
    EXPECT_EQ(text, stream.str());
}

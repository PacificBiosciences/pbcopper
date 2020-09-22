#include <pbcopper/utility/StringView.h>

#include <sstream>
#include <string>

#include <gtest/gtest.h>

TEST(Utility_StringView, Simple)
{
    PacBio::Utility::StringView sv1{"ACGT"};
    EXPECT_EQ(4, sv1.Size());
    EXPECT_EQ(4, sv1.Length());
    EXPECT_EQ('A', sv1[0]);
    EXPECT_EQ('C', sv1[1]);
    EXPECT_EQ('G', sv1[2]);
    EXPECT_EQ('T', sv1[3]);

    sv1 = sv1.Substr(1, 2);
    EXPECT_EQ('C', sv1[0]);
    EXPECT_EQ('G', sv1[1]);
    EXPECT_EQ('G', sv1.Back());

    const std::string str{"CG"};
    PacBio::Utility::StringView sv2{str};
    EXPECT_EQ(sv1, sv2);
}

TEST(Utility_StringView, Ostream)
{
    PacBio::Utility::StringView sv{"myDNA", 5};
    std::ostringstream ostr;
    ostr << sv;

    EXPECT_EQ("myDNA", ostr.str());
}

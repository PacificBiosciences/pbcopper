#include <pbcopper/utility/Base64.h>

#include <gtest/gtest.h>

// Adapted from: https://github.com/ReneNyffenegger/cpp-base64/blob/master/test-google.cpp

using namespace PacBio;

namespace Base64Tests {

void Roundtrip(const std::string& input, const std::string& expectedEncoded)
{
    const auto encoded = Utility::Base64::Encode(input);
    const auto decoded = Utility::Base64::Decode(encoded);
    EXPECT_EQ(expectedEncoded, encoded);
    EXPECT_EQ(input, decoded);
}

}  // namespace Base64Tests

TEST(Utility_Base64, can_roundtrip_with_no_fill_bytes)
{
    const std::string input{"abc"};
    const std::string encoded{"YWJj"};
    Base64Tests::Roundtrip(input, encoded);
}

TEST(Utility_Base64, can_roundtrip_with_one_fill_byte)
{
    const std::string input{"abcde"};
    const std::string encoded{"YWJjZGU="};
    Base64Tests::Roundtrip(input, encoded);
}

TEST(Utility_Base64, can_roundtrip_with_two_fill_bytes)
{
    const std::string input{"abcd"};
    const std::string encoded{"YWJjZA=="};
    Base64Tests::Roundtrip(input, encoded);
}

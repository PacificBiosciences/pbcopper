
#include <gtest/gtest.h>

#include <pbcopper/utility/FileUtils.h>

TEST(Utility_FileUtils, FileExists)
{
    ASSERT_FALSE(PacBio::Utility::FileExists("/dev/pacbio/doesnt/exist"));
}

TEST(Utility_FileUtils, FilePrefix)
{
    ASSERT_EQ("myfile", PacBio::Utility::FilePrefix("/dev/pacbio/myfile.txt"));
    ASSERT_EQ("prefix", PacBio::Utility::FilePrefix("/dev/pacbio/prefix.infix.suffix"));
    ASSERT_EQ("prefix", PacBio::Utility::FilePrefix("/prefix.infix.suffix"));
    ASSERT_EQ("prefix", PacBio::Utility::FilePrefix("prefix.infix.suffix"));
}

TEST(Utility_FileUtils, FileExtension)
{
    ASSERT_EQ("txt", PacBio::Utility::FileExtension("/dev/pacbio/myfile.txt"));
    ASSERT_EQ("suffix", PacBio::Utility::FileExtension("/dev/pacbio/prefix.infix.suffix"));
    ASSERT_EQ("suffix", PacBio::Utility::FileExtension("/prefix.infix.suffix"));
    ASSERT_EQ("suffix", PacBio::Utility::FileExtension("prefix.infix.suffix"));
}

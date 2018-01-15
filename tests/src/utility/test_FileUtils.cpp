

#include <gtest/gtest.h>
#include <pbcopper/utility/FileUtils.h>

using namespace PacBio;
using namespace PacBio::Utility;

TEST(Utility_FileUtils, FileExists) { ASSERT_FALSE(FileExists("/dev/pacbio/doesnt/exist")); }

TEST(Utility_FileUtils, FilePrefix)
{
    ASSERT_EQ("myfile", FilePrefix("/dev/pacbio/myfile.txt"));
    ASSERT_EQ("prefix", FilePrefix("/dev/pacbio/prefix.infix.suffix"));
    ASSERT_EQ("prefix", FilePrefix("/prefix.infix.suffix"));
    ASSERT_EQ("prefix", FilePrefix("prefix.infix.suffix"));
}

TEST(Utility_FileUtils, FileExtension)
{
    ASSERT_EQ("txt", FileExtension("/dev/pacbio/myfile.txt"));
    ASSERT_EQ("suffix", FileExtension("/dev/pacbio/prefix.infix.suffix"));
    ASSERT_EQ("suffix", FileExtension("/prefix.infix.suffix"));
    ASSERT_EQ("suffix", FileExtension("prefix.infix.suffix"));
}
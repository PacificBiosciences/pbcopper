#include <pbcopper/align/EdlibAlign.h>

#include <vector>

#include <gtest/gtest.h>

TEST(Align_EdlibAlignmentToCigar, empty_alignment_yields_empty_cigar)
{
    const std::vector<unsigned char> input;
    PacBio::Data::Cigar expected;

    const auto result = PacBio::Align::EdlibAlignmentToCigar(input.data(), input.size());
    EXPECT_EQ(expected, result);
}

TEST(Align_EdlibAlignmentToCigar, can_convert_single_op_edlib_alignment_to_cigar)
{
    const std::vector<unsigned char> input{EDLIB_EDOP_MATCH, EDLIB_EDOP_MATCH, EDLIB_EDOP_MATCH};
    const PacBio::Data::Cigar expected("3=");

    const auto result = PacBio::Align::EdlibAlignmentToCigar(input.data(), input.size());
    EXPECT_EQ(expected, result);
}

TEST(Align_EdlibAlignmentToCigar, can_convert_multiple_op_edlib_alignment_to_cigar)
{
    const std::vector<unsigned char> input{
        EDLIB_EDOP_MATCH,  EDLIB_EDOP_MATCH,  EDLIB_EDOP_MATCH,  EDLIB_EDOP_MISMATCH,
        EDLIB_EDOP_INSERT, EDLIB_EDOP_DELETE, EDLIB_EDOP_DELETE, EDLIB_EDOP_INSERT};
    const PacBio::Data::Cigar expected("3=1X1I2D1I");

    const auto result = PacBio::Align::EdlibAlignmentToCigar(input.data(), input.size());
    EXPECT_EQ(expected, result);
}

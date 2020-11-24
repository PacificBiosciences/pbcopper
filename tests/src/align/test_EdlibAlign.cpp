#include <pbcopper/align/EdlibAlign.h>

#include <string>
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

// clang-format off
TEST(Align_EdlibAlign, can_align_subreads_list)
{
    const std::string target{"ATATTAGGC"};
    const std::vector<std::string> queries{
        "ATATAGCCGGC",
      // ATATTAG--G-C
      // ATAT-AGCCGGC
      // 4=1D2=2I1=1I1=

        "ATATACGGC",
      // ATATTA-GGC
      // ATAT-ACGGC
      // 4=1D1=1I3=

        "ATCATCCGGC",
      // AT-ATTAGGC
      // ATCATCCGGC
      // 2=1I2=2X3=

        "ATATACCGAG",
      // ATATTAG-GC-
      // ATAT-ACCGAG
      // 4=1D1=1X1I1=1X1I

        "ATATAGCCGGC",
      // ATATTAG--G-C
      // ATAT-AGCCGGC
      // 4=1D2=2I1=1I1=
    };
    const EdlibAlignConfig config{-1, EDLIB_MODE_NW, EDLIB_TASK_PATH, nullptr, 0};
    const std::vector<std::string> expectedCigars{
        "4=1D2=2I1=1I1=",
        "4=1D1=1I3=",
        "2=1I2=2X3=",
        "4=1D1=1X1I1=1X1I",
        "4=1D2=2I1=1I1="
    };

    const auto alignments = PacBio::Align::EdlibAlign(queries, target, config);
    ASSERT_EQ(expectedCigars.size(), alignments.size());
    for (size_t i = 0; i < alignments.size(); ++i) {
        const auto& cigarStr = PacBio::Align::EdlibAlignmentToCigar(*alignments[i]).ToStdString();
        EXPECT_EQ(cigarStr, expectedCigars[i]);
    }
}
// clang-format on

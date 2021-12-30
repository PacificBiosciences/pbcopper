#include <pbcopper/align/GlobalLocalAlignment.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(Align_GlobalLocalAlignment, perfect_match)
{
    using namespace PacBio::Align;
    const std::string both{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2};
    GlobalLocalResult result = GlobalLocalAlign(both, both, params);
    EXPECT_EQ(params.MatchScore * both.size(), result.MaxScore);
    EXPECT_EQ(both.size(), result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_matrix)
{
    using namespace PacBio::Align;
    const std::string both{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2};
    std::vector<int32_t> matrix(0, both.size());
    GlobalLocalResult result = GlobalLocalAlign(both, both, params, matrix);
    EXPECT_EQ(params.MatchScore * both.size(), result.MaxScore);
    EXPECT_EQ(both.size(), result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_openend)
{
    using namespace PacBio::Align;
    const std::string target{"GATTACAT"};
    const std::string query{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2};
    GlobalLocalResult result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size(), result.MaxScore);
    EXPECT_EQ(query.size(), result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_openbegin)
{
    using namespace PacBio::Align;
    const std::string target{"TTAAGAGATTACA"};
    const std::string query{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2};
    GlobalLocalResult result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size(), result.MaxScore);
    EXPECT_EQ(target.size(), result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_infix)
{
    using namespace PacBio::Align;
    const std::string target{"TTAAGAGATTACATTTAT"};
    const std::string query{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2};
    GlobalLocalResult result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size(), result.MaxScore);
    EXPECT_EQ(13, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, homopolymer)
{
    using namespace PacBio::Align;
    const std::string target{"GATTACA"};
    const std::string query{"GATACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2};
    GlobalLocalResult result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size() + params.BranchPenalty, result.MaxScore);
    EXPECT_EQ(target.size(), result.EndPos);
}

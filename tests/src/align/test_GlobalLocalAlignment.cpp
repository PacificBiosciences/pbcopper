#include <pbcopper/align/GlobalLocalAlignment.h>

#include <pbcopper/utility/Stopwatch.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(Align_GlobalLocalAlignment, perfect_match)
{
    using namespace PacBio::Align;
    const std::string both{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2, -2};
    GlobalLocalResult result = GlobalLocalAlign(both, both, params);
    EXPECT_EQ(params.MatchScore * both.size(), result.MaxScore);
    EXPECT_EQ(both.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, deletion)
{
    using namespace PacBio::Align;
    const std::string read{"GTTACA"};
    const std::string qry{"GATTACA"};
    GlobalLocalParameters params{10, -4, -1, -3, -2, -2};
    GlobalLocalResult result = GlobalLocalAlign(qry, read, params);
    EXPECT_EQ(params.MatchScore * read.size() + params.DeletionPenalty, result.MaxScore);
    EXPECT_EQ(read.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, insertion)
{
    using namespace PacBio::Align;
    const std::string read{"GATTACA"};
    const std::string qry{"GTTACA"};
    GlobalLocalParameters params{10, -4, -1, -3, -2, -2};
    GlobalLocalResult result = GlobalLocalAlign(qry, read, params);
    EXPECT_EQ(params.MatchScore * qry.size() + params.InsertionPenalty, result.MaxScore);
    EXPECT_EQ(read.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, branch)
{
    using namespace PacBio::Align;
    const std::string read{"GATTAACA"};
    const std::string qry{"GATTACA"};
    GlobalLocalParameters params{10, -5, -4, -3, -1, -2};
    GlobalLocalResult result = GlobalLocalAlign(qry, read, params);
    EXPECT_EQ(params.MatchScore * qry.size() + params.BranchPenalty, result.MaxScore);
    EXPECT_EQ(read.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, merge)
{
    using namespace PacBio::Align;
    const std::string read{"GATACA"};
    const std::string qry{"GATTACA"};
    GlobalLocalParameters params{10, -5, -4, -3, -1, -2};
    GlobalLocalResult result = GlobalLocalAlign(qry, read, params);
    EXPECT_EQ(params.MatchScore * read.size() + params.MergePenalty, result.MaxScore);
    EXPECT_EQ(read.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_openend)
{
    using namespace PacBio::Align;
    const std::string target{"GATTACAT"};
    const std::string query{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2, -2};
    GlobalLocalResult result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size(), result.MaxScore);
    EXPECT_EQ(query.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_openbegin)
{
    using namespace PacBio::Align;
    const std::string target{"TTAAGAGATTACA"};
    const std::string query{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2, -2};
    GlobalLocalResult result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size(), result.MaxScore);
    EXPECT_EQ(target.size() - 1, result.EndPos);
}

TEST(Align_GlobalLocalAlignment, perfect_match_infix)
{
    using namespace PacBio::Align;
    const std::string target{"TTAAGAGATTACATTTAT"};
    const std::string query{"GATTACA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2, -2};
    GlobalLocalResult result;
    result = GlobalLocalAlign(query, target, params);
    EXPECT_EQ(params.MatchScore * query.size(), result.MaxScore);
    EXPECT_EQ(12, result.EndPos);
}

#if 0
TEST(Align_GlobalLocalAlignment, random)
{
    using namespace PacBio::Align;
    const std::string target{"AGTAGGAGTAGTTAGGTATAAA"};
    const std::string query{"GTGTTAAAGTGTTAAA"};
    std::string target2{
        "GTATGAGTAGAGGTAGTAGTATGTAGTAGTAGTGTAGTAGTGTAGTAGTAGTGTAGTAGTGTATGGATAGTAGAGTAGAT"};
    for (int32_t i = 0; i < 8; ++i) {
        target2 = target2 + target2;
    }
    std::cerr << target2.size() << '\n';
    const std::string query2{"TGATAGAGTATGAGTA"};
    GlobalLocalParameters params{4, -4, -3, -3, -2, -2};
    GlobalLocalResult result;
    std::vector<int32_t> matrix;
    PacBio::Utility::Stopwatch t;
    const int32_t rounds = 5000;
    GlobalLocalStorage storage{};
    for (int32_t i = 0; i < rounds * 1000; ++i) {
        result = GlobalLocalAlign(query, target, params, storage);
    }
    t.Freeze();
    int64_t time = t.ElapsedNanoseconds() / rounds / 1000;
    std::cerr << t.PrettyPrintNanoseconds(time) << '\n';
    t.Reset();
    for (int32_t i = 0; i < rounds; ++i) {
        result = GlobalLocalAlign(query2, target2, params, storage);
    }
    t.Freeze();
    time = t.ElapsedNanoseconds() / rounds;
    std::cerr << t.PrettyPrintNanoseconds(time) << '\n';
}
#endif

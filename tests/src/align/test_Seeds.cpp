
#include <pbcopper/align/Seeds.h>
#include <gtest/gtest.h>
using namespace PacBio::Align;

TEST(Align_Seed, construct_from_begin_positions_and_length)
{
    Seed s{ 1, 2, 3 };

    EXPECT_EQ(1, s.BeginPositionH());
    EXPECT_EQ(4, s.EndPositionH());
    EXPECT_EQ(2, s.BeginPositionV());
    EXPECT_EQ(5, s.EndPositionV());
    EXPECT_EQ(-1, s.UpperDiagonal());
    EXPECT_EQ(-1, s.LowerDiagonal());
    EXPECT_EQ(-1, s.BeginDiagonal());
    EXPECT_EQ(-1, s.EndDiagonal());

    s.LowerDiagonal(5);
    s.UpperDiagonal(42);

    EXPECT_EQ(5, s.LowerDiagonal());
    EXPECT_EQ(42, s.UpperDiagonal());
}

TEST(Align_Seed, construct_from_begin_and_end_positions)
{
    const Seed s{ 1, 2, 3, 5 };

    EXPECT_EQ(1, s.BeginPositionH());
    EXPECT_EQ(3, s.EndPositionH());
    EXPECT_EQ(2, s.BeginPositionV());
    EXPECT_EQ(5, s.EndPositionV());
    EXPECT_EQ(-1, s.UpperDiagonal());
    EXPECT_EQ(-2, s.LowerDiagonal());
    EXPECT_EQ(-1, s.BeginDiagonal());
    EXPECT_EQ(-2, s.EndDiagonal());
}

TEST(Align_Seed, check_combineable)
{
    {   // partial overlap
        const Seed s1{ 0,0,3 };
        const Seed s2{ 2,2,3 };
        EXPECT_TRUE(internal::CanMergeSeeds(s1, s2));
    }
    {   // complete overlap
        const Seed s1{ 0,0,3 };
        const Seed s2{ 0,0,3 };
        EXPECT_TRUE(internal::CanMergeSeeds(s1, s2));
    }
    {   // contained - prefix overlap
        const Seed s1{ 0,0,3 };
        const Seed s2{ 0,0,4 };
        EXPECT_TRUE(internal::CanMergeSeeds(s1, s2));
    }
    {   // contained - suffix overlap
        const Seed s1{ 0,0,3 };
        const Seed s2{ 1,1,3 };
        EXPECT_TRUE(internal::CanMergeSeeds(s1, s2));
    }
    {   // no overlap
        const Seed s1{ 0,0,4 };
        const Seed s2{ 5,5,3 };
        EXPECT_FALSE(internal::CanMergeSeeds(s1, s2));
    }
    {   // improper order
        const Seed s1{ 2,2,3 };
        const Seed s2{ 0,0,3 };
        EXPECT_FALSE(internal::CanMergeSeeds(s1, s2));
    }
}

TEST(Align_Seeds, default_construct)
{
    {
        Seeds seeds;
        EXPECT_EQ(seeds.begin(), seeds.end());
        EXPECT_EQ(0, seeds.size());
    }
    {
        const Seeds seeds;
        EXPECT_EQ(seeds.begin(), seeds.end());
        EXPECT_EQ(0, seeds.size());
    }
}

TEST(Align_Seeds, add_seed)
{
    Seeds seeds;
    seeds.AddSeed(Seed{1,2,3});

    EXPECT_EQ(1, seeds.size());
    auto iter = seeds.begin();
    ++iter;
    EXPECT_EQ(iter, seeds.end());

    EXPECT_EQ(Seed(1,2,3), *seeds.begin());
}

TEST(Align_Seeds, merge_seed_from_left)
{
    Seeds seeds;
    seeds.AddSeed(Seed{3,3,3});
    EXPECT_EQ(1, seeds.size());

    const auto mergedOk = seeds.TryMergeSeed(Seed{2,2,3});
    EXPECT_TRUE(mergedOk);
    EXPECT_EQ(1, seeds.size());

    const auto& s = *(seeds.cbegin());
    EXPECT_EQ(2, s.BeginPositionH());
    EXPECT_EQ(2, s.BeginPositionV());
    EXPECT_EQ(6, s.EndPositionH());
    EXPECT_EQ(6, s.EndPositionV());
    EXPECT_EQ(0, s.Score());
}

TEST(Align_Seeds, merge_seed_from_right)
{
    Seeds seeds;
    seeds.AddSeed(Seed{2,2,3});
    EXPECT_EQ(1, seeds.size());

    const auto mergedOk = seeds.TryMergeSeed(Seed{3,3,3});
    EXPECT_TRUE(mergedOk);
    EXPECT_EQ(1, seeds.size());

    const auto& s = *(seeds.cbegin());
    EXPECT_EQ(2, s.BeginPositionH());
    EXPECT_EQ(2, s.BeginPositionV());
    EXPECT_EQ(6, s.EndPositionH());
    EXPECT_EQ(6, s.EndPositionV());
    EXPECT_EQ(0, s.Score());
}

TEST(Align_Seeds, merge_not_possible_but_fallback_to_add_is_ok)
{
    Seeds seeds;
    seeds.AddSeed(Seed{0,0,4});
    EXPECT_EQ(1, seeds.size());

    const Seed s{5,5,3};
    const auto mergedOk = seeds.TryMergeSeed(s);
    EXPECT_FALSE(mergedOk);
    EXPECT_EQ(1, seeds.size());

    seeds.AddSeed(s);
    EXPECT_EQ(2, seeds.size());
}

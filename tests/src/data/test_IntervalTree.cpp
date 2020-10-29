#include <pbcopper/data/IntervalTree.h>

#include <cstddef>

#include <stdexcept>

#include <gtest/gtest.h>

#include <pbcopper/data/Interval.h>

using namespace PacBio::Data;

TEST(IntervalTest, Merging)
{
    IntervalTree tree;

    tree.Insert(Interval(1, 3));
    tree.Insert(Interval(3, 5));

    EXPECT_EQ(tree.size(), 1);

    for (const auto& i : tree) {
        EXPECT_EQ(i.End(), 5);
        EXPECT_EQ(i.Start(), 1);
    }
}

TEST(IntervalTest, Merging2)
{
    IntervalTree tree;

    tree.Insert(Interval(1, 3));
    tree.Insert(Interval(5, 7));
    tree.Insert(Interval(9, 11));

    EXPECT_EQ(tree.size(), 3);

    tree.Insert(Interval(3, 9));

    EXPECT_EQ(tree.size(), 1);

    for (const auto& i : tree) {
        EXPECT_EQ(i.End(), 11);
        EXPECT_EQ(i.Start(), 1);
    }
}

TEST(IntervalTest, Merging3)
{
    IntervalTree tree;

    tree.Insert(Interval(1, 3));
    tree.Insert(Interval(5, 6));
    tree.Insert(Interval(4, 6));

    EXPECT_EQ(tree.size(), 2);
}

TEST(IntervalTest, Iteration)
{
    Interval interval(0, 11);
    size_t i = interval.Start();

    for (const auto j : interval) {
        EXPECT_EQ(j, i++);
    }
}

TEST(IntervalTest, Gaps)
{
    IntervalTree tree;

    tree.Insert(Interval(1, 3));
    tree.Insert(Interval(5, 7));
    tree.Insert(Interval(9, 11));

    IntervalTree gaps = tree.Gaps();

    EXPECT_EQ(gaps.size(), 2);

    size_t l = 3;
    size_t r = 5;

    for (const auto& i : gaps) {
        EXPECT_EQ(i.Start(), l);
        EXPECT_EQ(i.End(), r);

        l += 4;
        r += 4;
    }
}

TEST(IntervalTest, Gaps2)
{
    IntervalTree tree;

    tree.Insert(Interval(3, 9));

    IntervalTree gaps = tree.Gaps(Interval(5, 11));

    EXPECT_EQ(gaps.size(), 1);

    for (auto& i : gaps) {
        EXPECT_EQ(i.Start(), 9);
        EXPECT_EQ(i.End(), 11);
    }

    gaps = tree.Gaps(Interval(1, 11));

    EXPECT_EQ(gaps.size(), 2);

    size_t l = 1;
    size_t r = 3;

    for (const auto& i : gaps) {
        EXPECT_EQ(i.Start(), l);
        EXPECT_EQ(i.End(), r);

        l += 8;
        r += 8;
    }

    gaps = tree.Gaps(Interval(11, 15));

    EXPECT_EQ(gaps.size(), 1);

    for (const auto& i : gaps) {
        EXPECT_EQ(i.Start(), 11);
        EXPECT_EQ(i.End(), 15);
    }
}

TEST(IntervalTest, Gaps3)
{
    IntervalTree tree;

    tree.Insert(Interval(3, 5));
    tree.Insert(Interval(7, 9));

    IntervalTree gaps = tree.Gaps(Interval(4, 9));

    EXPECT_EQ(gaps.size(), 1);

    for (const auto& i : gaps) {
        EXPECT_EQ(i.Start(), 5);
        EXPECT_EQ(i.End(), 7);
    }
}

TEST(IntervalTest, ZMW25300)
{
    IntervalTree tree;

    tree.Insert(Interval(252, 295));
    tree.Insert(Interval(293, 338));

    for (const auto& i : tree) {
        EXPECT_EQ(i.Start(), 252);
        EXPECT_EQ(i.End(), 338);
    }
}

TEST(IntervalTest, FromString)
{
    Interval a = Interval::FromString("1");

    EXPECT_EQ(a.Start(), 1);
    EXPECT_EQ(a.End(), 2);

    IntervalTree tree = IntervalTree::FromString("1,3-4");

    size_t l = 1, r = 2;

    for (const auto& i : tree) {
        EXPECT_EQ(i.Start(), l);
        EXPECT_EQ(i.End(), r);
        l = 3;
        r = 5;
    }

    EXPECT_THROW({ IntervalTree::FromString("A,15-22"); }, std::invalid_argument);

    EXPECT_THROW({ IntervalTree::FromString("15-2"); }, std::invalid_argument);

    tree = IntervalTree::FromString("2-2");

    for (const auto& i : tree) {
        EXPECT_EQ(i.Start(), 2);
        EXPECT_EQ(i.End(), 3);
    }
}

TEST(IntervalTest, Contains)
{
    Interval a = Interval::FromString("2");

    EXPECT_FALSE(a.Contains(1));
    EXPECT_TRUE(a.Contains(2));
    EXPECT_FALSE(a.Contains(3));

    IntervalTree tree = IntervalTree::FromString("5,8-10");

    EXPECT_FALSE(tree.Contains(4));
    EXPECT_TRUE(tree.Contains(5));
    EXPECT_FALSE(tree.Contains(6));

    EXPECT_FALSE(tree.Contains(7));
    EXPECT_TRUE(tree.Contains(8));
    EXPECT_TRUE(tree.Contains(9));
    EXPECT_TRUE(tree.Contains(10));
    EXPECT_FALSE(tree.Contains(11));
}

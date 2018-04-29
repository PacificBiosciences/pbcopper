
#include <gtest/gtest.h>
#include <pbcopper/data/Interval.h>
#include <pbcopper/data/Position.h>
using namespace PacBio;
using namespace PacBio::Data;

TEST(Data_Interval, default_interval_contains_zeroes)
{
    Interval<Position> empty;
    EXPECT_EQ(0, empty.Start());
    EXPECT_EQ(0, empty.End());
}

TEST(Data_Interval, construction_with_explicit_start_and_end_retained)
{
    Interval<Position> normal(5, 8);
    EXPECT_EQ(5, normal.Start());
    EXPECT_EQ(8, normal.End());
}

TEST(Data_Interval, construction_without_explict_endpoint_yields_endpoint_of_start_plus_one)
{
    Interval<Position> singleton(4);
    EXPECT_EQ(4, singleton.Start());
    EXPECT_EQ(5, singleton.End());
}

TEST(Data_Interval, self_equals_self)
{
    Interval<Position> empty;
    Interval<Position> singleton(4);
    Interval<Position> normal(5, 8);

    EXPECT_TRUE(empty == empty);
    EXPECT_TRUE(singleton == singleton);
    EXPECT_TRUE(normal == normal);
}

TEST(Data_Interval, intervals_with_same_endpoints_are_equal)
{
    Interval<Position> empty;
    Interval<Position> sameAsEmpty;
    Interval<Position> singleton(4);
    Interval<Position> sameAsSingleton(4, 5);
    Interval<Position> normal(5, 8);
    Interval<Position> sameAsNormal(5, 8);

    EXPECT_TRUE(empty == sameAsEmpty);
    EXPECT_TRUE(singleton == sameAsSingleton);
    EXPECT_TRUE(normal == sameAsNormal);
}

TEST(Data_Interval, intervals_with_different_endpoints_are_not_equal)
{
    Interval<Position> empty;
    Interval<Position> singleton(4);
    Interval<Position> normal(5, 8);
    Interval<Position> different(20, 40);

    EXPECT_FALSE(empty == singleton);
    EXPECT_FALSE(empty == normal);
    EXPECT_FALSE(empty == different);
    EXPECT_FALSE(singleton == normal);
    EXPECT_FALSE(normal == different);
}

TEST(Data_Interval, can_be_copy_constructed)
{
    Interval<Position> original(5, 8);
    Interval<Position> copy(original);
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, can_be_copy_assigned)
{
    Interval<Position> original(5, 8);
    Interval<Position> copy = original;
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, modification_of_copy_does_not_affect_original)
{
    Interval<Position> original(5, 8);
    Interval<Position> copy(original);
    copy.Start(2);
    copy.End(10);

    EXPECT_FALSE(original == copy);
    EXPECT_EQ(2, copy.Start());
    EXPECT_EQ(10, copy.End());
}

TEST(Data_Interval, self_covers_and_is_covered_by_self)
{
    Interval<Position> interval(2, 4);
    EXPECT_TRUE(interval.Covers(interval));
    EXPECT_TRUE(interval.CoveredBy(interval));
}

TEST(Data_Interval, covers_and_coveredby_are_reciprocal)
{
    Interval<Position> inner(3, 5);
    Interval<Position> outer(1, 7);

    EXPECT_TRUE(inner.CoveredBy(outer));  // a.coveredBy(b)
    EXPECT_TRUE(outer.Covers(inner));     // thus b.covers(a)

    EXPECT_FALSE(inner == outer);       // if a != b
    EXPECT_FALSE(inner.Covers(outer));  // then !a.covers(b)
}

TEST(Data_Interval, completely_disjoint_intervals_do_not_cover_each_other)
{
    Interval<Position> left(3, 5);
    Interval<Position> right(6, 8);

    EXPECT_FALSE(left.Covers(right));
    EXPECT_FALSE(right.Covers(left));
    EXPECT_FALSE(left.CoveredBy(right));
    EXPECT_FALSE(right.CoveredBy(left));
}

TEST(Data_Interval, no_coverage_when_left_stop_same_as_right_start)
{
    Interval<Position> left(3, 5);
    Interval<Position> right(5, 8);

    EXPECT_FALSE(left.Covers(right));
    EXPECT_FALSE(left.CoveredBy(right));
}

TEST(Data_Interval, coverage_when_endpoints_are_same_and_inner_start_contained_by_outer)
{
    Interval<Position> inner(6, 8);
    Interval<Position> outer(5, 8);

    EXPECT_TRUE(outer.Covers(inner));
    EXPECT_TRUE(inner.CoveredBy(outer));
}

TEST(Data_Interval, calculates_proper_intersection)
{
    Interval<Position> interval1(2, 4);
    Interval<Position> interval2(3, 5);
    Interval<Position> interval3(6, 8);
    Interval<Position> interval4(1, 7);
    Interval<Position> interval5(5, 8);

    EXPECT_TRUE(interval1.Intersects(interval1));  // self-intersection: a.intersects(a)

    EXPECT_TRUE(interval1.Intersects(interval2));  // if a.intersects(b)
    EXPECT_TRUE(interval2.Intersects(interval1));  // then b.intersects(a)

    EXPECT_TRUE(interval4.Covers(interval1));      // if b.covers(a),
    EXPECT_TRUE(interval1.Intersects(interval4));  // then a.intersects(b)
    EXPECT_TRUE(interval4.Intersects(interval1));  // and b.intersects(a)

    EXPECT_FALSE(interval2.Intersects(interval3));  // b.start > a.stop (obvious disjoint)
    EXPECT_FALSE(interval2.Intersects(
        interval5));  // b.start == a.stop (intervals are right open, so disjoint)
}

TEST(Data_Interval, acceptable_endpoints_are_valid)
{
    Interval<Position> zeroStartInterval(0, 1);
    Interval<Position> nonZeroStartInterval(4, 5);

    EXPECT_TRUE(zeroStartInterval.IsValid());
    EXPECT_TRUE(nonZeroStartInterval.IsValid());
}

TEST(Data_Interval, nonsensical_endpoints_are_invalid)
{
    Interval<Position> defaultConstructedInterval;
    Interval<Position> zeroEmptyValue(0, 0);
    Interval<Position> nonZeroEmptyInterval(4, 4);
    Interval<Position> badOrderingInterval(5, 4);

    EXPECT_FALSE(defaultConstructedInterval.IsValid());
    EXPECT_FALSE(zeroEmptyValue.IsValid());
    EXPECT_FALSE(nonZeroEmptyInterval.IsValid());
    EXPECT_FALSE(badOrderingInterval.IsValid());
}

TEST(Data_Interval, calculates_proper_length)
{
    Interval<Position> interval1(2, 4);
    Interval<Position> interval2(3, 5);
    Interval<Position> interval3(6, 8);
    Interval<Position> interval4(1, 7);
    Interval<Position> interval5(5, 8);

    EXPECT_EQ(2, interval1.Length());
    EXPECT_EQ(2, interval2.Length());
    EXPECT_EQ(2, interval3.Length());
    EXPECT_EQ(6, interval4.Length());
    EXPECT_EQ(3, interval5.Length());
}

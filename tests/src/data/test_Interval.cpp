#include <pbcopper/data/Interval.h>

#include <gtest/gtest.h>

#include <pbcopper/data/Position.h>

TEST(Data_Interval, default_interval_contains_zeroes)
{
    PacBio::Data::Interval<PacBio::Data::Position> empty;
    EXPECT_EQ(0, empty.Start());
    EXPECT_EQ(0, empty.End());
}

TEST(Data_Interval, construction_with_explicit_start_and_end_retained)
{
    PacBio::Data::Interval<PacBio::Data::Position> normal(5, 8);
    EXPECT_EQ(5, normal.Start());
    EXPECT_EQ(8, normal.End());
}

TEST(Data_Interval, construction_without_explict_endpoint_yields_endpoint_of_start_plus_one)
{
    PacBio::Data::Interval<PacBio::Data::Position> singleton(4);
    EXPECT_EQ(4, singleton.Start());
    EXPECT_EQ(5, singleton.End());
}

TEST(Data_Interval, self_equals_self)
{
    PacBio::Data::Interval<PacBio::Data::Position> empty;
    PacBio::Data::Interval<PacBio::Data::Position> singleton(4);
    PacBio::Data::Interval<PacBio::Data::Position> normal(5, 8);

    EXPECT_TRUE(empty == empty);
    EXPECT_TRUE(singleton == singleton);
    EXPECT_TRUE(normal == normal);
}

TEST(Data_Interval, intervals_with_same_endpoints_are_equal)
{
    PacBio::Data::Interval<PacBio::Data::Position> empty;
    PacBio::Data::Interval<PacBio::Data::Position> sameAsEmpty;
    PacBio::Data::Interval<PacBio::Data::Position> singleton(4);
    PacBio::Data::Interval<PacBio::Data::Position> sameAsSingleton(4, 5);
    PacBio::Data::Interval<PacBio::Data::Position> normal(5, 8);
    PacBio::Data::Interval<PacBio::Data::Position> sameAsNormal(5, 8);

    EXPECT_TRUE(empty == sameAsEmpty);
    EXPECT_TRUE(singleton == sameAsSingleton);
    EXPECT_TRUE(normal == sameAsNormal);
}

TEST(Data_Interval, intervals_with_different_endpoints_are_not_equal)
{
    PacBio::Data::Interval<PacBio::Data::Position> empty;
    PacBio::Data::Interval<PacBio::Data::Position> singleton(4);
    PacBio::Data::Interval<PacBio::Data::Position> normal(5, 8);
    PacBio::Data::Interval<PacBio::Data::Position> different(20, 40);

    EXPECT_FALSE(empty == singleton);
    EXPECT_FALSE(empty == normal);
    EXPECT_FALSE(empty == different);
    EXPECT_FALSE(singleton == normal);
    EXPECT_FALSE(normal == different);
}

TEST(Data_Interval, can_be_copy_constructed)
{
    PacBio::Data::Interval<PacBio::Data::Position> original(5, 8);
    PacBio::Data::Interval<PacBio::Data::Position> copy(original);
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, can_be_copy_assigned)
{
    PacBio::Data::Interval<PacBio::Data::Position> original(5, 8);
    PacBio::Data::Interval<PacBio::Data::Position> copy = original;
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, modification_of_copy_does_not_affect_original)
{
    PacBio::Data::Interval<PacBio::Data::Position> original(5, 8);
    PacBio::Data::Interval<PacBio::Data::Position> copy(original);
    copy.Start(2);
    copy.End(10);

    EXPECT_FALSE(original == copy);
    EXPECT_EQ(2, copy.Start());
    EXPECT_EQ(10, copy.End());
}

TEST(Data_Interval, self_covers_and_is_covered_by_self)
{
    PacBio::Data::Interval<PacBio::Data::Position> interval(2, 4);
    EXPECT_TRUE(interval.Covers(interval));
    EXPECT_TRUE(interval.CoveredBy(interval));
}

TEST(Data_Interval, covers_and_coveredby_are_reciprocal)
{
    PacBio::Data::Interval<PacBio::Data::Position> inner(3, 5);
    PacBio::Data::Interval<PacBio::Data::Position> outer(1, 7);

    EXPECT_TRUE(inner.CoveredBy(outer));  // a.coveredBy(b)
    EXPECT_TRUE(outer.Covers(inner));     // thus b.covers(a)

    EXPECT_FALSE(inner == outer);       // if a != b
    EXPECT_FALSE(inner.Covers(outer));  // then !a.covers(b)
}

TEST(Data_Interval, completely_disjoint_intervals_do_not_cover_each_other)
{
    PacBio::Data::Interval<PacBio::Data::Position> left(3, 5);
    PacBio::Data::Interval<PacBio::Data::Position> right(6, 8);

    EXPECT_FALSE(left.Covers(right));
    EXPECT_FALSE(right.Covers(left));
    EXPECT_FALSE(left.CoveredBy(right));
    EXPECT_FALSE(right.CoveredBy(left));
}

TEST(Data_Interval, no_coverage_when_left_stop_same_as_right_start)
{
    PacBio::Data::Interval<PacBio::Data::Position> left(3, 5);
    PacBio::Data::Interval<PacBio::Data::Position> right(5, 8);

    EXPECT_FALSE(left.Covers(right));
    EXPECT_FALSE(left.CoveredBy(right));
}

TEST(Data_Interval, coverage_when_endpoints_are_same_and_inner_start_contained_by_outer)
{
    PacBio::Data::Interval<PacBio::Data::Position> inner(6, 8);
    PacBio::Data::Interval<PacBio::Data::Position> outer(5, 8);

    EXPECT_TRUE(outer.Covers(inner));
    EXPECT_TRUE(inner.CoveredBy(outer));
}

TEST(Data_Interval, calculates_proper_intersection)
{
    PacBio::Data::Interval<PacBio::Data::Position> interval1(2, 4);
    PacBio::Data::Interval<PacBio::Data::Position> interval2(3, 5);
    PacBio::Data::Interval<PacBio::Data::Position> interval3(6, 8);
    PacBio::Data::Interval<PacBio::Data::Position> interval4(1, 7);
    PacBio::Data::Interval<PacBio::Data::Position> interval5(5, 8);

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
    PacBio::Data::Interval<PacBio::Data::Position> zeroStartInterval(0, 1);
    PacBio::Data::Interval<PacBio::Data::Position> nonZeroStartInterval(4, 5);

    EXPECT_TRUE(zeroStartInterval.IsValid());
    EXPECT_TRUE(nonZeroStartInterval.IsValid());
}

TEST(Data_Interval, nonsensical_endpoints_are_invalid)
{
    PacBio::Data::Interval<PacBio::Data::Position> defaultConstructedInterval;
    PacBio::Data::Interval<PacBio::Data::Position> zeroEmptyValue(0, 0);
    PacBio::Data::Interval<PacBio::Data::Position> nonZeroEmptyInterval(4, 4);
    PacBio::Data::Interval<PacBio::Data::Position> badOrderingInterval(5, 4);

    EXPECT_FALSE(defaultConstructedInterval.IsValid());
    EXPECT_FALSE(zeroEmptyValue.IsValid());
    EXPECT_FALSE(nonZeroEmptyInterval.IsValid());
    EXPECT_FALSE(badOrderingInterval.IsValid());
}

TEST(Data_Interval, calculates_proper_length)
{
    PacBio::Data::Interval<PacBio::Data::Position> interval1(2, 4);
    PacBio::Data::Interval<PacBio::Data::Position> interval2(3, 5);
    PacBio::Data::Interval<PacBio::Data::Position> interval3(6, 8);
    PacBio::Data::Interval<PacBio::Data::Position> interval4(1, 7);
    PacBio::Data::Interval<PacBio::Data::Position> interval5(5, 8);

    EXPECT_EQ(2, interval1.Length());
    EXPECT_EQ(2, interval2.Length());
    EXPECT_EQ(2, interval3.Length());
    EXPECT_EQ(6, interval4.Length());
    EXPECT_EQ(3, interval5.Length());
}

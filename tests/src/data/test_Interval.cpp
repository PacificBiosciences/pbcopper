#include <pbcopper/data/Interval.h>

#include <gtest/gtest.h>

TEST(Data_Interval, default_interval_contains_zeroes)
{
    const PacBio::Data::Interval empty;
    EXPECT_EQ(0, empty.Start());
    EXPECT_EQ(0, empty.End());
}

TEST(Data_Interval, construction_with_explicit_start_and_end_retained)
{
    const PacBio::Data::Interval normal{5, 8};
    EXPECT_EQ(5, normal.Start());
    EXPECT_EQ(8, normal.End());
}

TEST(Data_Interval, construction_without_explict_endpoint_yields_endpoint_of_start_plus_one)
{
    const PacBio::Data::Interval singleton{4};
    EXPECT_EQ(4, singleton.Start());
    EXPECT_EQ(5, singleton.End());
}

TEST(Data_Interval, self_equals_self)
{
    const PacBio::Data::Interval empty;
    const PacBio::Data::Interval singleton{4};
    const PacBio::Data::Interval normal{5, 8};

    EXPECT_TRUE(empty == empty);
    EXPECT_TRUE(singleton == singleton);
    EXPECT_TRUE(normal == normal);
}

TEST(Data_Interval, intervals_with_same_endpoints_are_equal)
{
    const PacBio::Data::Interval empty;
    const PacBio::Data::Interval sameAsEmpty;
    const PacBio::Data::Interval singleton{4};
    const PacBio::Data::Interval sameAsSingleton{4, 5};
    const PacBio::Data::Interval normal{5, 8};
    const PacBio::Data::Interval sameAsNormal{5, 8};

    EXPECT_TRUE(empty == sameAsEmpty);
    EXPECT_TRUE(singleton == sameAsSingleton);
    EXPECT_TRUE(normal == sameAsNormal);
}

TEST(Data_Interval, intervals_with_different_endpoints_are_not_equal)
{
    const PacBio::Data::Interval empty;
    const PacBio::Data::Interval singleton{4};
    const PacBio::Data::Interval normal{5, 8};
    const PacBio::Data::Interval different{20, 40};

    EXPECT_FALSE(empty == singleton);
    EXPECT_FALSE(empty == normal);
    EXPECT_FALSE(empty == different);
    EXPECT_FALSE(singleton == normal);
    EXPECT_FALSE(normal == different);
}

TEST(Data_Interval, can_be_copy_constructed)
{
    const PacBio::Data::Interval original{5, 8};
    const PacBio::Data::Interval copy{original};
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, can_be_copy_assigned)
{
    const PacBio::Data::Interval original{5, 8};
    const PacBio::Data::Interval copy = original;
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, modification_of_copy_does_not_affect_original)
{
    const PacBio::Data::Interval original{5, 8};
    PacBio::Data::Interval copy{original};
    copy.Start(2);
    copy.End(10);

    EXPECT_FALSE(original == copy);
    EXPECT_EQ(2, copy.Start());
    EXPECT_EQ(10, copy.End());
}

TEST(Data_Interval, self_covers_and_is_covered_by_self)
{
    const PacBio::Data::Interval interval{2, 4};
    EXPECT_TRUE(interval.Contains(interval));
    EXPECT_TRUE(interval.ContainedBy(interval));
}

TEST(Data_Interval, covers_and_coveredby_are_reciprocal)
{
    const PacBio::Data::Interval inner{3, 5};
    const PacBio::Data::Interval outer{1, 7};

    EXPECT_TRUE(inner.ContainedBy(outer));  // a.coveredBy(b)
    EXPECT_TRUE(outer.Contains(inner));     // thus b.covers(a)

    EXPECT_FALSE(inner == outer);         // if a != b
    EXPECT_FALSE(inner.Contains(outer));  // then !a.covers(b)
}

TEST(Data_Interval, completely_disjoint_intervals_do_not_cover_each_other)
{
    const PacBio::Data::Interval left{3, 5};
    const PacBio::Data::Interval right{6, 8};

    EXPECT_FALSE(left.Contains(right));
    EXPECT_FALSE(right.Contains(left));
    EXPECT_FALSE(left.ContainedBy(right));
    EXPECT_FALSE(right.ContainedBy(left));
}

TEST(Data_Interval, no_coverage_when_left_stop_same_as_right_start)
{
    const PacBio::Data::Interval left{3, 5};
    const PacBio::Data::Interval right{5, 8};

    EXPECT_FALSE(left.Contains(right));
    EXPECT_FALSE(left.ContainedBy(right));
}

TEST(Data_Interval, coverage_when_endpoints_are_same_and_inner_start_contained_by_outer)
{
    const PacBio::Data::Interval inner{6, 8};
    const PacBio::Data::Interval outer{5, 8};

    EXPECT_TRUE(outer.Contains(inner));
    EXPECT_TRUE(inner.ContainedBy(outer));
}

TEST(Data_Interval, calculates_proper_intersection)
{
    const PacBio::Data::Interval interval1{2, 4};
    const PacBio::Data::Interval interval2{3, 5};
    const PacBio::Data::Interval interval3{6, 8};
    const PacBio::Data::Interval interval4{1, 7};
    const PacBio::Data::Interval interval5{5, 8};

    EXPECT_TRUE(interval1.Intersects(interval1));  // self-intersection: a.intersects(a)

    EXPECT_TRUE(interval1.Intersects(interval2));  // if a.intersects(b)
    EXPECT_TRUE(interval2.Intersects(interval1));  // then b.intersects(a)

    EXPECT_TRUE(interval4.Contains(interval1));    // if b.covers(a),
    EXPECT_TRUE(interval1.Intersects(interval4));  // then a.intersects(b)
    EXPECT_TRUE(interval4.Intersects(interval1));  // and b.intersects(a)

    EXPECT_FALSE(interval2.Intersects(interval3));  // b.start > a.stop (obvious disjoint)
    EXPECT_FALSE(interval2.Intersects(
        interval5));  // b.start == a.stop (intervals are right open, so disjoint)
}

TEST(Data_Interval, acceptable_endpoints_are_valid)
{
    const PacBio::Data::Interval zeroStartInterval{0, 1};
    const PacBio::Data::Interval nonZeroStartInterval{4, 5};

    EXPECT_TRUE(zeroStartInterval.IsValid());
    EXPECT_TRUE(nonZeroStartInterval.IsValid());
}

TEST(Data_Interval, nonsensical_endpoints_are_invalid)
{
    const PacBio::Data::Interval defaultConstructedInterval;
    const PacBio::Data::Interval zeroEmptyValue{0, 0};
    const PacBio::Data::Interval nonZeroEmptyInterval{4, 4};
    const PacBio::Data::Interval badOrderingInterval{5, 4};

    EXPECT_FALSE(defaultConstructedInterval.IsValid());
    EXPECT_FALSE(zeroEmptyValue.IsValid());
    EXPECT_FALSE(nonZeroEmptyInterval.IsValid());
    EXPECT_FALSE(badOrderingInterval.IsValid());
}

TEST(Data_Interval, calculates_proper_length)
{
    const PacBio::Data::Interval interval1{2, 4};
    const PacBio::Data::Interval interval2{3, 5};
    const PacBio::Data::Interval interval3{6, 8};
    const PacBio::Data::Interval interval4{1, 7};
    const PacBio::Data::Interval interval5{5, 8};

    EXPECT_EQ(2, interval1.Length());
    EXPECT_EQ(2, interval2.Length());
    EXPECT_EQ(2, interval3.Length());
    EXPECT_EQ(6, interval4.Length());
    EXPECT_EQ(3, interval5.Length());
}

TEST(Data_Interval, constructors)
{
    const PacBio::Data::Interval empty;
    const PacBio::Data::Interval singleton{4};
    const PacBio::Data::Interval normal{5, 8};

    EXPECT_EQ(0, empty.Start());
    EXPECT_EQ(0, empty.End());

    EXPECT_EQ(4, singleton.Start());
    EXPECT_EQ(5, singleton.End());

    EXPECT_EQ(5, normal.Start());
    EXPECT_EQ(8, normal.End());

    // TODO: check out-of-order intervals, etc
}

TEST(Data_Interval, equality_test)
{
    const PacBio::Data::Interval empty;
    const PacBio::Data::Interval empty2;

    const PacBio::Data::Interval singleton{4};
    const PacBio::Data::Interval sameAsSingleton{4, 5};

    const PacBio::Data::Interval normal{5, 8};
    const PacBio::Data::Interval sameAsNormal{5, 8};

    const PacBio::Data::Interval different{20, 40};

    // self-equality
    EXPECT_TRUE(empty == empty);
    EXPECT_TRUE(singleton == singleton);
    EXPECT_TRUE(normal == normal);
    EXPECT_TRUE(different == different);

    // same values equality
    EXPECT_TRUE(empty == empty2);
    EXPECT_TRUE(singleton == sameAsSingleton);
    EXPECT_TRUE(normal == sameAsNormal);

    // different values
    EXPECT_FALSE(empty == singleton);
    EXPECT_FALSE(empty == normal);
    EXPECT_FALSE(empty == different);
    EXPECT_FALSE(singleton == normal);
    EXPECT_FALSE(normal == different);
}

TEST(Data_Interval, copy)
{
    const PacBio::Data::Interval interval1{5, 8};
    const PacBio::Data::Interval interval2{interval1};
    const PacBio::Data::Interval interval3 = interval1;

    EXPECT_TRUE(interval1 == interval1);
    EXPECT_TRUE(interval1 == interval2);
    EXPECT_TRUE(interval1 == interval3);
}

TEST(Data_Interval, modifier)
{
    const PacBio::Data::Interval interval1{5, 8};
    PacBio::Data::Interval interval2{interval1};
    interval2.Start(2);
    interval2.End(10);

    EXPECT_FALSE(interval1 == interval2);
    EXPECT_EQ(2, interval2.Start());
    EXPECT_EQ(10, interval2.End());
}

TEST(Data_Interval, cover_test)
{
    const PacBio::Data::Interval interval1{2, 4};
    const PacBio::Data::Interval interval2{3, 5};
    const PacBio::Data::Interval interval3{6, 8};
    const PacBio::Data::Interval interval4{1, 7};
    const PacBio::Data::Interval interval5{5, 8};

    EXPECT_TRUE(interval1.Contains(interval1));     // self-cover: a.covers(a)
    EXPECT_TRUE(interval1.ContainedBy(interval1));  // self-cover: a.coveredBy(a)

    EXPECT_TRUE(interval2.ContainedBy(interval4));  // a.coveredBy(b)
    EXPECT_TRUE(interval4.Contains(interval2));     // thus b.covers(a)
    EXPECT_FALSE(interval2 == interval4);           // if a != b
    EXPECT_FALSE(interval2.Contains(interval4));    // then !a.covers(b)

    EXPECT_FALSE(interval2.Contains(interval3));  // completely disjoint
    EXPECT_FALSE(interval3.Contains(interval2));
    EXPECT_FALSE(interval2.ContainedBy(interval3));
    EXPECT_FALSE(interval3.ContainedBy(interval2));

    EXPECT_FALSE(interval2.Contains(interval5));  // a.stop == b.start
    EXPECT_FALSE(interval2.ContainedBy(interval5));

    EXPECT_TRUE(
        interval5.Contains(interval3));  // shared endpoint, start contained, thus a.covers(b)
    EXPECT_TRUE(interval3.ContainedBy(interval5));  // and b.coveredBy(a)
}

TEST(Data_Interval, intersect_test)
{
    const PacBio::Data::Interval interval1{2, 4};
    const PacBio::Data::Interval interval2{3, 5};
    const PacBio::Data::Interval interval3{6, 8};
    const PacBio::Data::Interval interval4{1, 7};
    const PacBio::Data::Interval interval5{5, 8};

    EXPECT_TRUE(interval1.Intersects(interval1));  // self-intersection: a.intersects(a)

    EXPECT_TRUE(interval1.Intersects(interval2));  // if a.intersects(b)
    EXPECT_TRUE(interval2.Intersects(interval1));  // then b.intersects(a)

    EXPECT_TRUE(interval4.Contains(interval1));    // if b.covers(a),
    EXPECT_TRUE(interval1.Intersects(interval4));  // then a.intersects(b)
    EXPECT_TRUE(interval4.Intersects(interval1));  // and b.intersects(a)

    EXPECT_FALSE(interval2.Intersects(interval3));  // b.start > a.stop (obvious disjoint)
    EXPECT_FALSE(interval2.Intersects(
        interval5));  // b.start == a.stop (intervals are right open, so disjoint)
}

TEST(Data_Interval, validity_test)
{
    const PacBio::Data::Interval interval1;        // default ctor
    const PacBio::Data::Interval interval2{0, 0};  // start == stop (zero)
    const PacBio::Data::Interval interval3{4, 4};  // start == stop (nonzero)
    const PacBio::Data::Interval interval4{0, 1};  // start < stop  (start is zero)
    const PacBio::Data::Interval interval5{4, 5};  // start < stop  (start is nonzero)
    const PacBio::Data::Interval interval6{5, 4};  // start > stop

    EXPECT_FALSE(interval1.IsValid());
    EXPECT_FALSE(interval2.IsValid());
    EXPECT_FALSE(interval3.IsValid());
    EXPECT_TRUE(interval4.IsValid());
    EXPECT_TRUE(interval5.IsValid());
    EXPECT_FALSE(interval6.IsValid());
}

TEST(Data_Interval, length_test)
{
    const PacBio::Data::Interval interval1{2, 4};
    const PacBio::Data::Interval interval2{3, 5};
    const PacBio::Data::Interval interval3{6, 8};
    const PacBio::Data::Interval interval4{1, 7};
    const PacBio::Data::Interval interval5{5, 8};

    EXPECT_EQ(2, interval1.Length());
    EXPECT_EQ(2, interval2.Length());
    EXPECT_EQ(2, interval3.Length());
    EXPECT_EQ(6, interval4.Length());
    EXPECT_EQ(3, interval5.Length());

    // TODO: check out-of-order intervals, etc
}

// Author: Derek Barnett

#include <string>

#include <gtest/gtest.h>

#include <pbcopper/data/GenomicInterval.h>

using namespace PacBio;

TEST(Data_GenomicInterval, DefaultConstructor)
{
    Data::GenomicInterval gi;
    EXPECT_EQ("", gi.Name());
    EXPECT_EQ(0, gi.Start());
    EXPECT_EQ(0, gi.Stop());
}

TEST(Data_GenomicInterval, ExplicitConstructor)
{
    Data::GenomicInterval gi("foo", 100, 200);
    EXPECT_EQ("foo", gi.Name());
    EXPECT_EQ(100, gi.Start());
    EXPECT_EQ(200, gi.Stop());
}

TEST(Data_GenomicInterval, RegionStringConstructor)
{
    Data::GenomicInterval gi("foo:100-200");
    EXPECT_EQ("foo", gi.Name());
    EXPECT_EQ(100, gi.Start());
    EXPECT_EQ(200, gi.Stop());

    Data::GenomicInterval refOnly("foo");
    EXPECT_EQ("foo", refOnly.Name());
    EXPECT_EQ(0, refOnly.Start());
    EXPECT_EQ(1 << 29, refOnly.Stop());  // htslib's default, "read-to-end" interval stop
}

TEST(Data_GenomicInterval, Copy)
{
    Data::GenomicInterval interval1("foo", 10, 20);
    Data::GenomicInterval interval2(interval1);
    Data::GenomicInterval interval3 = interval1;

    EXPECT_TRUE(interval1 == interval1);
    EXPECT_TRUE(interval1 == interval2);
    EXPECT_TRUE(interval1 == interval3);
}

TEST(Data_GenomicInterval, Modifiers)
{
    Data::GenomicInterval interval1("foo", 10, 20);

    // modify individual properties
    Data::GenomicInterval interval2(interval1);
    interval2.Name("bar");
    interval2.Start(2);
    interval2.Stop(10);

    // modify interval as a whole
    Data::GenomicInterval interval3(interval1);
    interval3.Interval(interval2.Interval());

    EXPECT_FALSE(interval1 == interval2);
    EXPECT_EQ("bar", interval2.Name());
    EXPECT_EQ(2, interval2.Start());
    EXPECT_EQ(10, interval2.Stop());

    EXPECT_EQ(interval1.Name(), interval3.Name());
    EXPECT_EQ(interval2.Interval(), interval3.Interval());
}

TEST(Data_GenomicInterval, CoverTest)
{
    Data::GenomicInterval interval1("foo", 2, 4);
    Data::GenomicInterval interval2("foo", 3, 5);
    Data::GenomicInterval interval3("foo", 6, 8);
    Data::GenomicInterval interval4("foo", 1, 7);
    Data::GenomicInterval interval5("foo", 5, 8);

    // same as interval2, but different ref
    Data::GenomicInterval interval6(interval2);
    interval6.Name("bar");

    EXPECT_TRUE(interval1.Covers(interval1));     // self-cover: a.covers(a)
    EXPECT_TRUE(interval1.CoveredBy(interval1));  // self-cover: a.coveredBy(a)

    EXPECT_TRUE(interval2.CoveredBy(interval4));  // a.coveredBy(b)
    EXPECT_TRUE(interval4.Covers(interval2));     // thus b.covers(a)
    EXPECT_FALSE(interval2 == interval4);         // if a != b
    EXPECT_FALSE(interval2.Covers(interval4));    // then !a.covers(b)

    EXPECT_FALSE(
        interval6.CoveredBy(interval4));  // interval 6 has same start/stop as 2, w/ different ref
    EXPECT_FALSE(interval4.Covers(interval6));  //
    EXPECT_FALSE(interval6 == interval4);       //
    EXPECT_FALSE(interval6.Covers(interval4));  //

    EXPECT_FALSE(interval2.Covers(interval3));  // completely disjoint
    EXPECT_FALSE(interval3.Covers(interval2));
    EXPECT_FALSE(interval2.CoveredBy(interval3));
    EXPECT_FALSE(interval3.CoveredBy(interval2));

    EXPECT_FALSE(interval2.Covers(interval5));  // a.stop == b.start
    EXPECT_FALSE(interval2.CoveredBy(interval5));

    EXPECT_TRUE(interval5.Covers(interval3));  // shared endpoint, start contained, thus a.covers(b)
    EXPECT_TRUE(interval3.CoveredBy(interval5));  // and b.coveredBy(a)
}

TEST(Data_GenomicInterval, ValidityTest)
{
    Data::GenomicInterval interval1;               // default ctor
    Data::GenomicInterval interval2("foo", 0, 0);  // valid id, start == stop (zero)
    Data::GenomicInterval interval3("foo", 4, 4);  // valid id, start == stop (nonzero)
    Data::GenomicInterval interval4("foo", 0, 1);  // valid id, start < stop  (start is zero)
    Data::GenomicInterval interval5("foo", 4, 5);  // valid id, start < stop  (start is nonzero)
    Data::GenomicInterval interval6("foo", 5, 4);  // valid id, start > stop
    Data::GenomicInterval interval7("", 0, 0);     // invalid id, start == stop (zero)
    Data::GenomicInterval interval8("", 4, 4);     // invalid id, start == stop (nonzero)
    Data::GenomicInterval interval9("", 0, 1);     // invalid id, start < stop  (start is zero)
    Data::GenomicInterval interval10("", 4, 5);    // invalid id, start < stop  (start is nonzero)
    Data::GenomicInterval interval11("", 5, 4);    // invalid id, start > stop

    EXPECT_FALSE(interval1.IsValid());
    EXPECT_FALSE(interval2.IsValid());
    EXPECT_FALSE(interval3.IsValid());
    EXPECT_TRUE(interval4.IsValid());
    EXPECT_TRUE(interval5.IsValid());
    EXPECT_FALSE(interval6.IsValid());
    EXPECT_FALSE(interval7.IsValid());
    EXPECT_FALSE(interval8.IsValid());
    EXPECT_FALSE(interval9.IsValid());
    EXPECT_FALSE(interval10.IsValid());
    EXPECT_FALSE(interval11.IsValid());
}


#include <pbcopper/data/ReadName.h>
#include <gtest/gtest.h>
using namespace PacBio;
using namespace PacBio::Data;
using namespace std;

TEST(Data_ReadName, read_name_from_string)
{
    const string movieName = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    const Zmw zmw = 553;
    const Position queryStart = 3100;
    const Position queryEnd   = 11230;
    const Interval<Position> queryInterval{ queryStart, queryEnd };

    const ReadName readName("m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/3100_11230");

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw,       readName.Zmw());

    EXPECT_EQ(queryInterval,         readName.QueryInterval());
    EXPECT_EQ(queryInterval.Start(), readName.QueryStart());
    EXPECT_EQ(queryInterval.End(),   readName.QueryEnd());

    EXPECT_FALSE(readName.IsCCS());
}

TEST(Data_ReadName, ccs_read_name_from_string)
{
    const string movieName = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    const Zmw zmw = 553;
    const Position queryStart = 3100;
    const Position queryEnd   = 11230;
    const Interval<Position> queryInterval{ queryStart, queryEnd };

    const ReadName readName("m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/ccs");

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw,       readName.Zmw());

    EXPECT_TRUE(readName.IsCCS());

    EXPECT_THROW(readName.QueryEnd(),      std::runtime_error);
    EXPECT_THROW(readName.QueryInterval(), std::runtime_error);
    EXPECT_THROW(readName.QueryStart(),    std::runtime_error);
}

TEST(Data_ReadName, read_name_from_parts_with_query_interval)
{
    const string movieName = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    const Zmw zmw = 553;
    const Position queryStart = 3100;
    const Position queryEnd   = 11230;
    const Interval<Position> queryInterval{ queryStart, queryEnd };

    const ReadName readName{ MovieName{movieName}, zmw, queryInterval };

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw,       readName.Zmw());

    EXPECT_EQ(queryInterval,         readName.QueryInterval());
    EXPECT_EQ(queryInterval.Start(), readName.QueryStart());
    EXPECT_EQ(queryInterval.End(),   readName.QueryEnd());

    EXPECT_FALSE(readName.IsCCS());
}

TEST(Data_ReadName, read_name_from_parts_with_query_positions)
{
    const string movieName = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    const Zmw zmw = 553;
    const Position queryStart = 3100;
    const Position queryEnd   = 11230;
    const Interval<Position> queryInterval{ queryStart, queryEnd };

    const ReadName readName{ MovieName{movieName}, zmw, queryStart, queryEnd };

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw,       readName.Zmw());

    EXPECT_EQ(queryInterval,         readName.QueryInterval());
    EXPECT_EQ(queryInterval.Start(), readName.QueryStart());
    EXPECT_EQ(queryInterval.End(),   readName.QueryEnd());

    EXPECT_FALSE(readName.IsCCS());
}

TEST(Data_ReadName, ccs_read_name_from_parts)
{
    const string movieName = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    const Zmw zmw = 553;
    const Position queryStart = 3100;
    const Position queryEnd   = 11230;
    const Interval<Position> queryInterval{ queryStart, queryEnd };

    const ReadName readName{ MovieName{movieName}, zmw, CCSTag() };

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw,       readName.Zmw());

    EXPECT_TRUE(readName.IsCCS());

    EXPECT_THROW(readName.QueryEnd(),      std::runtime_error);
    EXPECT_THROW(readName.QueryInterval(), std::runtime_error);
    EXPECT_THROW(readName.QueryStart(),    std::runtime_error);
}


TEST(Data_ReadName, compares_equal_if_printed_names_equal)
{
    const ReadName r("m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/3100_11230");
    const ReadName r2("m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/3100_11230");
    const ReadName r3("m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/ccs");
    const ReadName r4("m140415_143853_42175_c100635972550000001823121909121417_s1_p0/222/3100_11230");

    EXPECT_TRUE(r == r2);
    EXPECT_FALSE(r == r3);
    EXPECT_FALSE(r == r4);

    EXPECT_FALSE(r != r2);
    EXPECT_TRUE(r != r3);
    EXPECT_TRUE(r != r4);
}

TEST(Data_ReadName, constructed_from_name_prints_expected_value_to_output_operator)
{
    const string name = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/3100_11230";

    const ReadName r(name);
    stringstream s;
    s << r;
    EXPECT_EQ(name, s.str());
}

TEST(Data_ReadName, constructed_from_name_parts_prints_expected_value_to_output_operator)
{
    const string name = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/3100_11230";

    const ReadName r{ MovieName{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"},
                      553,
                      Interval<Position>{3100, 11230} };

    stringstream s;
    s << r;
    EXPECT_EQ(name, s.str());
}

TEST(Data_ReadName, constructed_from_ccs_name_parts_prints_expected_value_to_output_operator)
{
    const string ccs = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/ccs";

    const ReadName r{ MovieName{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"},
                      553,
                      CCSTag() };

    stringstream s;
    s << r;
    EXPECT_EQ(ccs, s.str());
}

TEST(Data_ReadName, constructed_properly_from_input_operator)
{
    const string name = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/3100_11230";

    stringstream s(name);
    ReadName r;
    s >> r;

    EXPECT_EQ("m140415_143853_42175_c100635972550000001823121909121417_s1_p0", r.MovieName().ToStdString());
    EXPECT_EQ(553,   r.Zmw());
    EXPECT_EQ(3100,  r.QueryStart());
    EXPECT_EQ(11230, r.QueryEnd());
    EXPECT_FALSE(r.IsCCS());
}

TEST(Data_ReadName, ccs_constructed_properly_from_input_operator)
{
    const string ccs = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0/553/ccs";

    stringstream s(ccs);
    ReadName r;
    s >> r;

    EXPECT_EQ("m140415_143853_42175_c100635972550000001823121909121417_s1_p0", r.MovieName().ToStdString());
    EXPECT_EQ(553,   r.Zmw());
    EXPECT_TRUE(r.IsCCS());
}


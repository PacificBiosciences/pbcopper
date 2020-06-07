#include <gtest/gtest.h>

#include <sstream>

#include <pbcopper/data/ReadName.h>

TEST(Data_ReadName, read_name_from_string)
{
    const std::string movieName{"m54001_160623_195125"};
    const PacBio::Data::Zmw zmw = 553;
    const PacBio::Data::Position queryStart = 3100;
    const PacBio::Data::Position queryEnd = 11230;
    const PacBio::Data::Interval queryInterval{queryStart, queryEnd};

    const PacBio::Data::ReadName readName{"m54001_160623_195125/553/3100_11230"};

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw, readName.Zmw());

    EXPECT_EQ(queryInterval, readName.QueryInterval());
    EXPECT_EQ(queryInterval.Start(), readName.QueryStart());
    EXPECT_EQ(queryInterval.End(), readName.QueryEnd());

    EXPECT_FALSE(readName.IsCCS());
}

TEST(Data_ReadName, ccs_read_name_from_string)
{
    const std::string movieName{"m54001_160623_195125"};
    const PacBio::Data::Zmw zmw = 553;
    const PacBio::Data::Position queryStart = 3100;
    const PacBio::Data::Position queryEnd = 11230;
    const PacBio::Data::Interval queryInterval{queryStart, queryEnd};

    const PacBio::Data::ReadName readName{"m54001_160623_195125/553/ccs"};

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw, readName.Zmw());

    EXPECT_TRUE(readName.IsCCS());

    EXPECT_THROW(readName.QueryEnd(), std::runtime_error);
    EXPECT_THROW(readName.QueryInterval(), std::runtime_error);
    EXPECT_THROW(readName.QueryStart(), std::runtime_error);
}

TEST(Data_ReadName, read_name_from_parts_with_query_interval)
{
    const std::string movieName{"m54001_160623_195125"};
    const PacBio::Data::Zmw zmw = 553;
    const PacBio::Data::Position queryStart = 3100;
    const PacBio::Data::Position queryEnd = 11230;
    const PacBio::Data::Interval queryInterval{queryStart, queryEnd};

    const PacBio::Data::ReadName readName{PacBio::Data::MovieName{movieName}, zmw, queryInterval};

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw, readName.Zmw());

    EXPECT_EQ(queryInterval, readName.QueryInterval());
    EXPECT_EQ(queryInterval.Start(), readName.QueryStart());
    EXPECT_EQ(queryInterval.End(), readName.QueryEnd());

    EXPECT_FALSE(readName.IsCCS());
}

TEST(Data_ReadName, read_name_from_parts_with_query_positions)
{
    const std::string movieName{"m54001_160623_195125"};
    const PacBio::Data::Zmw zmw = 553;
    const PacBio::Data::Position queryStart = 3100;
    const PacBio::Data::Position queryEnd = 11230;
    const PacBio::Data::Interval queryInterval{queryStart, queryEnd};

    const PacBio::Data::ReadName readName{PacBio::Data::MovieName{movieName}, zmw, queryStart,
                                          queryEnd};

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw, readName.Zmw());

    EXPECT_EQ(queryInterval, readName.QueryInterval());
    EXPECT_EQ(queryInterval.Start(), readName.QueryStart());
    EXPECT_EQ(queryInterval.End(), readName.QueryEnd());

    EXPECT_FALSE(readName.IsCCS());
}

TEST(Data_ReadName, ccs_read_name_from_parts)
{
    const std::string movieName{"m54001_160623_195125"};
    const PacBio::Data::Zmw zmw = 553;
    const PacBio::Data::Position queryStart = 3100;
    const PacBio::Data::Position queryEnd = 11230;
    const PacBio::Data::Interval queryInterval{queryStart, queryEnd};

    const PacBio::Data::ReadName readName{PacBio::Data::MovieName{movieName}, zmw,
                                          PacBio::Data::CCSTag()};

    EXPECT_EQ(movieName, readName.MovieName().ToStdString());
    EXPECT_EQ(zmw, readName.Zmw());

    EXPECT_TRUE(readName.IsCCS());

    EXPECT_THROW(readName.QueryEnd(), std::runtime_error);
    EXPECT_THROW(readName.QueryInterval(), std::runtime_error);
    EXPECT_THROW(readName.QueryStart(), std::runtime_error);
}

TEST(Data_ReadName, compares_equal_if_printed_names_equal)
{
    const PacBio::Data::ReadName r{"m54001_160623_195125/553/3100_11230"};
    const PacBio::Data::ReadName r2{"m54001_160623_195125/553/3100_11230"};
    const PacBio::Data::ReadName r3{"m54001_160623_195125/553/ccs"};
    const PacBio::Data::ReadName r4{"m54001_160623_195125/222/3100_11230"};

    EXPECT_EQ(r, r2);
    EXPECT_NE(r, r3);
    EXPECT_NE(r, r4);
}

TEST(Data_ReadName, constructed_from_name_prints_expected_value_to_output_operator)
{
    const std::string name{"m54001_160623_195125/553/3100_11230"};

    const PacBio::Data::ReadName r{name};
    std::ostringstream s;
    s << r;
    EXPECT_EQ(name, s.str());
}

TEST(Data_ReadName, constructed_from_name_parts_prints_expected_value_to_output_operator)
{
    const std::string name{"m54001_160623_195125/553/3100_11230"};

    const PacBio::Data::ReadName r{PacBio::Data::MovieName{"m54001_160623_195125"}, 553,
                                   PacBio::Data::Interval{3100, 11230}};

    std::ostringstream s;
    s << r;
    EXPECT_EQ(name, s.str());
}

TEST(Data_ReadName, constructed_from_ccs_name_parts_prints_expected_value_to_output_operator)
{
    const std::string ccs{"m54001_160623_195125/553/ccs"};

    const PacBio::Data::ReadName r{PacBio::Data::MovieName{"m54001_160623_195125"}, 553,
                                   PacBio::Data::CCSTag()};

    std::ostringstream s;
    s << r;
    EXPECT_EQ(ccs, s.str());
}

TEST(Data_ReadName, constructed_properly_from_input_operator)
{
    const std::string name{"m54001_160623_195125/553/3100_11230"};

    std::istringstream s{name};
    PacBio::Data::ReadName r;
    s >> r;

    EXPECT_EQ("m54001_160623_195125", r.MovieName().ToStdString());
    EXPECT_EQ(553, r.Zmw());
    EXPECT_EQ(3100, r.QueryStart());
    EXPECT_EQ(11230, r.QueryEnd());
    EXPECT_FALSE(r.IsCCS());
}

TEST(Data_ReadName, ccs_constructed_properly_from_input_operator)
{
    const std::string ccs{"m54001_160623_195125/553/ccs"};

    std::istringstream s{ccs};
    PacBio::Data::ReadName r;
    s >> r;

    EXPECT_EQ("m54001_160623_195125", r.MovieName().ToStdString());
    EXPECT_EQ(553, r.Zmw());
    EXPECT_TRUE(r.IsCCS());
}

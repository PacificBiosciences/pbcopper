#include <pbcopper/data/RSMovieName.h>

#include <sstream>

#include <gtest/gtest.h>

namespace RSMovieNameTests {

auto makeRSMovieName = [](void) {
    return PacBio::Data::RSMovieName{
        "m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};
};

}  // namespace RSMovieNameTests

TEST(Data_RSMovieName, construct_from_lvalue_string)
{
    EXPECT_NO_THROW({
        const std::string input{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};
        const PacBio::Data::RSMovieName m(input);
        EXPECT_EQ("140415_143853", m.RunStartTime());
        EXPECT_EQ("42175", m.InstrumentSerialNumber());
        EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
        EXPECT_EQ("s1", m.SetNumber());
        EXPECT_EQ("p0", m.PartNumber());
        EXPECT_FALSE(m.IsReagentExpired());
    });
}

TEST(Data_RSMovieName, construct_from_rvalue_string)
{
    EXPECT_NO_THROW({
        const PacBio::Data::RSMovieName m(
            "m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
        EXPECT_EQ("140415_143853", m.RunStartTime());
        EXPECT_EQ("42175", m.InstrumentSerialNumber());
        EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
        EXPECT_EQ("s1", m.SetNumber());
        EXPECT_EQ("p0", m.PartNumber());
        EXPECT_FALSE(m.IsReagentExpired());
    });
}

TEST(Data_RSMovieName, construct_from_name_parts)
{
    EXPECT_NO_THROW({
        const PacBio::Data::RSMovieName m("140415_143853", "42175",
                                          "c100635972550000001823121909121417", "s1", "p0");
        EXPECT_EQ("140415_143853", m.RunStartTime());
        EXPECT_EQ("42175", m.InstrumentSerialNumber());
        EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
        EXPECT_EQ("s1", m.SetNumber());
        EXPECT_EQ("p0", m.PartNumber());
        EXPECT_FALSE(m.IsReagentExpired());
    });
}

TEST(Data_RSMovieName, copy_constructor)
{
    const PacBio::Data::RSMovieName m(
        "m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());

    const PacBio::Data::RSMovieName m2(m);
    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_RSMovieName, copy_assign)
{
    const PacBio::Data::RSMovieName m(
        "m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());

    PacBio::Data::RSMovieName m2("mDummy_Dummy_Dummy_Dummy_Dummy_Dummy");
    m2 = m;

    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_RSMovieName, move_constructor)
{
    const PacBio::Data::RSMovieName m(RSMovieNameTests::makeRSMovieName());
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());
}

TEST(Data_RSMovieName, move_assign)
{
    PacBio::Data::RSMovieName m2("mDummy_Dummy_Dummy_Dummy_Dummy_Dummy");
    m2 = RSMovieNameTests::makeRSMovieName();

    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_RSMovieName, expired_reagent_character_recognized)
{
    {  // not expired
        const PacBio::Data::RSMovieName m(
            "m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
        EXPECT_FALSE(m.IsReagentExpired());
    }
    {  // expired
        const PacBio::Data::RSMovieName m(
            "m140415_143853_42175_c100635972550000001823121909121417_s1_X0");
        EXPECT_TRUE(m.IsReagentExpired());
    }
}

TEST(Data_RSMovieName, constructed_from_name_prints_same_name)
{
    const std::string expected{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};

    const PacBio::Data::RSMovieName m(expected);
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_RSMovieName, constructed_from_name_parts_prints_expected_combination)
{
    const std::string expected{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};

    const PacBio::Data::RSMovieName m("140415_143853", "42175",
                                      "c100635972550000001823121909121417", "s1", "p0");
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_RSMovieName, compares_equal_if_printed_names_equal)
{
    const PacBio::Data::RSMovieName m(
        "m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    const PacBio::Data::RSMovieName m2(
        "m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    const PacBio::Data::RSMovieName m3(
        "m140415_143853_42175_c100635972550000001823121909121417_s1_p1");

    EXPECT_TRUE(m == m2);
    EXPECT_FALSE(m != m2);

    EXPECT_FALSE(m == m3);
    EXPECT_TRUE(m != m3);
}

TEST(Data_RSMovieName, constructed_from_name_prints_expected_value_to_output_operator)
{
    const std::string expected{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};

    const PacBio::Data::RSMovieName m(expected);
    std::ostringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_RSMovieName, constructed_from_name_parts_prints_expected_value_to_output_operator)
{
    const std::string expected{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};

    const PacBio::Data::RSMovieName m("140415_143853", "42175",
                                      "c100635972550000001823121909121417", "s1", "p0");
    std::ostringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_RSMovieName, constructed_properly_from_input_operator)
{
    const std::string name{"m140415_143853_42175_c100635972550000001823121909121417_s1_p0"};
    std::istringstream s{name};
    PacBio::Data::RSMovieName m;
    s >> m;

    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());
}

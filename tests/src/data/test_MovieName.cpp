
#include <pbcopper/data/MovieName.h>
#include <gtest/gtest.h>
#include <sstream>
using namespace PacBio;
using namespace PacBio::Data;
using namespace std;

namespace PacBio {
namespace Data {
namespace tests {

auto makeMovieName = [](void)
{
    return MovieName{ "m140415_143853_42175_c100635972550000001823121909121417_s1_p0" };
};

} // namespace tests
} // namespace Data
} // namespace PacBio

TEST(Data_MovieName, construct_from_lvalue_string)
{
    EXPECT_NO_THROW(
    {
        const string input = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
        const MovieName m(input);
        EXPECT_EQ("140415_143853", m.RunStartTime());
        EXPECT_EQ("42175", m.InstrumentSerialNumber());
        EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
        EXPECT_EQ("s1", m.SetNumber());
        EXPECT_EQ("p0", m.PartNumber());
        EXPECT_FALSE(m.IsReagentExpired());
    });
}

TEST(Data_MovieName, construct_from_rvalue_string)
{
    EXPECT_NO_THROW(
    {
        const MovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
        EXPECT_EQ("140415_143853", m.RunStartTime());
        EXPECT_EQ("42175", m.InstrumentSerialNumber());
        EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
        EXPECT_EQ("s1", m.SetNumber());
        EXPECT_EQ("p0", m.PartNumber());
        EXPECT_FALSE(m.IsReagentExpired());
    });
}

TEST(Data_MovieName, construct_from_name_parts)
{
    EXPECT_NO_THROW(
    {
        const MovieName m("140415_143853",
                         "42175",
                         "c100635972550000001823121909121417",
                         "s1",
                         "p0");
        EXPECT_EQ("140415_143853", m.RunStartTime());
        EXPECT_EQ("42175", m.InstrumentSerialNumber());
        EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
        EXPECT_EQ("s1", m.SetNumber());
        EXPECT_EQ("p0", m.PartNumber());
        EXPECT_FALSE(m.IsReagentExpired());
    });
}

TEST(Data_MovieName, copy_constructor)
{
    const MovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());

    const MovieName m2(m);
    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_MovieName, copy_assign)
{
    const MovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());

    MovieName m2("mDummy_Dummy_Dummy_Dummy_Dummy_Dummy");
    m2 = m;

    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_MovieName, move_constructor)
{
    const MovieName m(tests::makeMovieName());
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());
}

TEST(Data_MovieName, move_assign)
{
    MovieName m2("mDummy_Dummy_Dummy_Dummy_Dummy_Dummy");
    m2 = tests::makeMovieName();

    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_MovieName, expired_reagent_character_recognized)
{
    {   // not expired
        const MovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
        EXPECT_FALSE(m.IsReagentExpired());
    }
    {   // expired
        const MovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_X0");
        EXPECT_TRUE(m.IsReagentExpired());
    }
}

TEST(Data_MovieName, constructed_from_name_prints_same_name)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const MovieName m(expected);
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_MovieName, constructed_from_name_parts_prints_expected_combination)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const MovieName m("140415_143853",
                     "42175",
                     "c100635972550000001823121909121417",
                     "s1",
                     "p0");
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_MovieName, compares_equal_if_printed_names_equal)
{
    const MovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    const MovieName m2("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    const MovieName m3("m140415_143853_42175_c100635972550000001823121909121417_s1_p1");

    EXPECT_TRUE(m == m2);
    EXPECT_FALSE(m != m2);

    EXPECT_FALSE(m == m3);
    EXPECT_TRUE(m != m3);
}

TEST(Data_MovieName, constructed_from_name_prints_expected_value_to_output_operator)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const MovieName m(expected);
    stringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_MovieName, constructed_from_name_parts_prints_expected_value_to_output_operator)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const MovieName m("140415_143853",
                      "42175",
                      "c100635972550000001823121909121417",
                      "s1",
                      "p0");
    stringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_MovieName, constructed_properly_from_input_operator)
{
    const string name = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    stringstream s(name);
    MovieName m;
    s >> m;

    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());
}

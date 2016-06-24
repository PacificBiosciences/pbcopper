
#include <pbcopper/data/RSMovieName.h>
#include <gtest/gtest.h>
#include <sstream>
using namespace PacBio;
using namespace PacBio::Data;
using namespace std;

namespace PacBio {
namespace Data {
namespace tests {

auto makeRSMovieName = [](void)
{
    return RSMovieName{ "m140415_143853_42175_c100635972550000001823121909121417_s1_p0" };
};

} // namespace tests
} // namespace Data
} // namespace PacBio

TEST(Data_RSMovieName, construct_from_lvalue_string)
{
    EXPECT_NO_THROW(
    {
        const string input = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
        const RSMovieName m(input);
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
    EXPECT_NO_THROW(
    {
        const RSMovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
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
    EXPECT_NO_THROW(
    {
        const RSMovieName m("140415_143853",
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

TEST(Data_RSMovieName, copy_constructor)
{
    const RSMovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());

    const RSMovieName m2(m);
    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_RSMovieName, copy_assign)
{
    const RSMovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());

    RSMovieName m2("mDummy_Dummy_Dummy_Dummy_Dummy_Dummy");
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
    const RSMovieName m(tests::makeRSMovieName());
    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());
}

TEST(Data_RSMovieName, move_assign)
{
    RSMovieName m2("mDummy_Dummy_Dummy_Dummy_Dummy_Dummy");
    m2 = tests::makeRSMovieName();

    EXPECT_EQ("140415_143853", m2.RunStartTime());
    EXPECT_EQ("42175", m2.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m2.SMRTCellBarcode());
    EXPECT_EQ("s1", m2.SetNumber());
    EXPECT_EQ("p0", m2.PartNumber());
    EXPECT_FALSE(m2.IsReagentExpired());
}

TEST(Data_RSMovieName, expired_reagent_character_recognized)
{
    {   // not expired
        const RSMovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
        EXPECT_FALSE(m.IsReagentExpired());
    }
    {   // expired
        const RSMovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_X0");
        EXPECT_TRUE(m.IsReagentExpired());
    }
}

TEST(Data_RSMovieName, constructed_from_name_prints_same_name)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const RSMovieName m(expected);
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_RSMovieName, constructed_from_name_parts_prints_expected_combination)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const RSMovieName m("140415_143853",
                     "42175",
                     "c100635972550000001823121909121417",
                     "s1",
                     "p0");
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_RSMovieName, compares_equal_if_printed_names_equal)
{
    const RSMovieName m("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    const RSMovieName m2("m140415_143853_42175_c100635972550000001823121909121417_s1_p0");
    const RSMovieName m3("m140415_143853_42175_c100635972550000001823121909121417_s1_p1");

    EXPECT_TRUE(m == m2);
    EXPECT_FALSE(m != m2);

    EXPECT_FALSE(m == m3);
    EXPECT_TRUE(m != m3);
}

TEST(Data_RSMovieName, constructed_from_name_prints_expected_value_to_output_operator)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const RSMovieName m(expected);
    stringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_RSMovieName, constructed_from_name_parts_prints_expected_value_to_output_operator)
{
    const string expected = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";

    const RSMovieName m("140415_143853",
                      "42175",
                      "c100635972550000001823121909121417",
                      "s1",
                      "p0");
    stringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_RSMovieName, constructed_properly_from_input_operator)
{
    const string name = "m140415_143853_42175_c100635972550000001823121909121417_s1_p0";
    stringstream s(name);
    RSMovieName m;
    s >> m;

    EXPECT_EQ("140415_143853", m.RunStartTime());
    EXPECT_EQ("42175", m.InstrumentSerialNumber());
    EXPECT_EQ("c100635972550000001823121909121417", m.SMRTCellBarcode());
    EXPECT_EQ("s1", m.SetNumber());
    EXPECT_EQ("p0", m.PartNumber());
    EXPECT_FALSE(m.IsReagentExpired());
}

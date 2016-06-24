
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
    return MovieName{ "m54001_160623_195125" };
};

} // namespace tests
} // namespace Data
} // namespace PacBio

TEST(Data_MovieName, construct_from_lvalue_string)
{
    EXPECT_NO_THROW(
    {
        const string input = "m54001_160623_195125";
        const MovieName m(input);
        EXPECT_EQ("160623_195125", m.RunStartTime());
        EXPECT_EQ("54001", m.InstrumentName());
    });
}

TEST(Data_MovieName, construct_from_rvalue_string)
{
    EXPECT_NO_THROW(
    {
        const MovieName m("m54001_160623_195125");
        EXPECT_EQ("160623_195125", m.RunStartTime());
        EXPECT_EQ("54001", m.InstrumentName());
    });
}

TEST(Data_MovieName, construct_from_name_parts)
{
    EXPECT_NO_THROW(
    {
        const MovieName m("54001",
                         "160623_195125");
        EXPECT_EQ("160623_195125", m.RunStartTime());
        EXPECT_EQ("54001", m.InstrumentName());
    });
}

TEST(Data_MovieName, copy_constructor)
{
    const MovieName m("m54001_160623_195125");
    EXPECT_EQ("160623_195125", m.RunStartTime());
    EXPECT_EQ("54001", m.InstrumentName());

    const MovieName m2(m);
    EXPECT_EQ("160623_195125", m2.RunStartTime());
    EXPECT_EQ("54001", m2.InstrumentName());
}

TEST(Data_MovieName, copy_assign)
{
    const MovieName m("m54001_160623_195125");
    EXPECT_EQ("160623_195125", m.RunStartTime());
    EXPECT_EQ("54001", m.InstrumentName());

    MovieName m2("mDummy_Dummy_Dummy");
    m2 = m;

    EXPECT_EQ("160623_195125", m2.RunStartTime());
    EXPECT_EQ("54001", m2.InstrumentName());
}

TEST(Data_MovieName, move_constructor)
{
    const MovieName m(tests::makeMovieName());
    EXPECT_EQ("160623_195125", m.RunStartTime());
    EXPECT_EQ("54001", m.InstrumentName());
}

TEST(Data_MovieName, move_assign)
{
    MovieName m2("mDummy_Dummy_Dummy");
    m2 = tests::makeMovieName();

    EXPECT_EQ("160623_195125", m2.RunStartTime());
    EXPECT_EQ("54001", m2.InstrumentName());
}

TEST(Data_MovieName, constructed_from_name_prints_same_name)
{
    const string expected = "m54001_160623_195125";

    const MovieName m(expected);
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_MovieName, constructed_from_name_parts_prints_expected_combination)
{
    const string expected = "m54001_160623_195125";

    const MovieName m("54001", "160623_195125");
    EXPECT_EQ(expected, m.ToStdString());
}

TEST(Data_MovieName, compares_equal_if_printed_names_equal)
{
    const MovieName m("m54001_160623_195125");
    const MovieName m2("m54001_160623_195125");
    const MovieName m3("m10000_160623_195125");

    EXPECT_TRUE(m == m2);
    EXPECT_FALSE(m != m2);

    EXPECT_FALSE(m == m3);
    EXPECT_TRUE(m != m3);
}

TEST(Data_MovieName, constructed_from_name_prints_expected_value_to_output_operator)
{
    const string expected = "m54001_160623_195125";

    const MovieName m(expected);
    stringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_MovieName, constructed_from_name_parts_prints_expected_value_to_output_operator)
{
    const string expected = "m54001_160623_195125";

    const MovieName m("54001",
                      "160623_195125");
    stringstream s;
    s << m;

    EXPECT_EQ(expected, s.str());
}

TEST(Data_MovieName, constructed_properly_from_input_operator)
{
    const string name = "m54001_160623_195125";
    stringstream s(name);
    MovieName m;
    s >> m;

    EXPECT_EQ("160623_195125", m.RunStartTime());
    EXPECT_EQ("54001", m.InstrumentName());
}

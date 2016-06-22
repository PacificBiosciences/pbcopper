
#include <pbcopper/cli/Option.h>
#include <gtest/gtest.h>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

TEST(CLI_Option, minimal_ctor_single_alias)
{
    Option opt{"opt_id", "o", ""};
    EXPECT_EQ(string("opt_id"), opt.Id());
    EXPECT_EQ(vector<string>{"o"}, opt.Names());
    EXPECT_FALSE(opt.DefaultValue());
    EXPECT_TRUE(opt.Description().empty());
//    EXPECT_TRUE(opt.ValueName().empty());
//    EXPECT_FALSE(opt.IsHidden());
}

TEST(CLI_Option, can_be_constructed_from_name_list_only)
{
    Option opt{"opt_id",  {"o", "output"}, "" };
    EXPECT_EQ(string("opt_id"), opt.Id());
    const vector<string> expected{"o", "output"};
    EXPECT_EQ(expected, opt.Names());
    EXPECT_FALSE(opt.DefaultValue());
    EXPECT_TRUE(opt.Description().empty());
//    EXPECT_TRUE(opt.ValueName().empty());
//    EXPECT_FALSE(opt.IsHidden());
}

TEST(CLI_Option, can_be_constructed_from_name_and_extra_info)
{
    Option opt{"opt_id", "o", "write data to <file>", Option::StringType("default.txt") };
    EXPECT_EQ(string("opt_id"), opt.Id());
    EXPECT_EQ(vector<string>{"o"},   opt.Names());
    EXPECT_EQ(string{"default.txt"}, opt.DefaultValue());
    EXPECT_EQ(string{ "write data to <file>"}, opt.Description());
//    EXPECT_FALSE(opt.IsHidden());
}

TEST(CLI_Option, can_be_constructed_from_name_list_and_extra_info)
{
    Option opt{"opt_id", {"o", "output"}, "write data to <file>", Option::StringType("default.txt") };
    const vector<string> expected{"o", "output"};
    EXPECT_EQ(expected, opt.Names());
    EXPECT_EQ(string{"default.txt"}, opt.DefaultValue());
    EXPECT_EQ(string{ "write data to <file>"}, opt.Description());
//    EXPECT_EQ(string{"file"}, opt.ValueName());
}

TEST(CLI_Option, rejects_empty_id)
{
    EXPECT_THROW(Option("", "name", "descr"), std::runtime_error);
    EXPECT_THROW(Option("", vector<string>{"name"}, "descr"), std::runtime_error);
}

TEST(CLI_Option, rejects_empty_names)
{
    EXPECT_THROW(Option("opt_id", "", "descrip"), std::runtime_error);
    EXPECT_THROW(Option("opt_id", vector<string>{}, "descr"), std::runtime_error);
}

TEST(CLI_Option, rejects_names_beginning_with_dash)
{
    EXPECT_THROW(Option("opt_id", "-o", "desc"), std::runtime_error);
    EXPECT_THROW(Option("opt_id", {"o", "-b"}, ""), std::runtime_error);
}

TEST(CLI_Option, rejects_names_beginning_with_slash)
{
    EXPECT_THROW(Option("opt_id", "/A", ""), std::runtime_error);
    EXPECT_THROW(Option("opt_id", {"o", "/X"}, ""), std::runtime_error);
}

TEST(CLI_Option, rejects_names_containing_equal)
{
    EXPECT_THROW(Option("opt_id", "foo=bar", ""), std::runtime_error);
    EXPECT_THROW(Option("opt_id", {"o", "foo=bar"}, "" ), std::runtime_error);
}

TEST(CLI_Option, can_be_copy_constructed)
{
    Option original{"opt_id", "o", "write data to <file>", Option::StringType("default.txt")};
    Option copy(original);

    EXPECT_EQ(original.Names(),        copy.Names());
    EXPECT_EQ(original.DefaultValue(), copy.DefaultValue());
    EXPECT_EQ(original.Description(),  copy.Description());
//    EXPECT_EQ(original.ValueName(),    copy.ValueName());
//    EXPECT_EQ(original.IsHidden(),     copy.IsHidden());
}

TEST(CLI_Option, can_be_copy_assigned)
{
    Option original{"opt_id", "o", "write data to <file>", Option::StringType("default.txt")};
    Option copy("opt_id2", "dummy", "");
    copy = original;

    EXPECT_EQ(original.Names(),        copy.Names());
    EXPECT_EQ(original.DefaultValue(), copy.DefaultValue());
    EXPECT_EQ(original.Description(),  copy.Description());
//    EXPECT_EQ(original.ValueName(),    copy.ValueName());
//    EXPECT_EQ(original.IsHidden(),     copy.IsHidden());
}

TEST(CLI_Option, can_be_move_constructed)
{
    Option original{"opt_id", "o", "write data to <file>", Option::StringType("default.txt")};
    Option destination(std::move(original));

    // ensure new option matches original's values (we can't touch original now)
    EXPECT_EQ(vector<string>{"o"}, destination.Names());
    EXPECT_EQ(string{"default.txt"}, destination.DefaultValue());
    EXPECT_EQ(string{ "write data to <file>"}, destination.Description());
//    EXPECT_EQ(string{"file"}, destination.ValueName());
//    EXPECT_FALSE(destination.IsHidden());
}

TEST(CLI_Option, can_be_move_assigned)
{
    Option original{"opt_id", "o", "write data to <file>", Option::StringType("default.txt")};
    Option destination("opt_id2", "dummy", "");
    destination = std::move(original);

    // ensure new option matches original's values (we can't touch original now)
    EXPECT_EQ(vector<string>{"o"}, destination.Names());
    EXPECT_EQ(string{"default.txt"}, destination.DefaultValue());
    EXPECT_EQ(string{ "write data to <file>"}, destination.Description());
//    EXPECT_EQ(string{"file"}, destination.ValueName());
//    EXPECT_FALSE(destination.IsHidden());
}

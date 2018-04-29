
#include <gtest/gtest.h>
#include <pbcopper/stream/Stream.h>
#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "OStreamRedirector.h"
using namespace PacBio;
using namespace PacBio::Stream;

namespace StreamTests {

Source<char> stringInput = [](Sink<char> s) {
    const std::string original = {"Test String "};
    for (auto e : original)
        s(e);
};

Source<char> newlinedStringInput = [](Sink<char> s) {
    const std::string original = {"Some\nfile\nwith\nnewlines\n"};
    for (auto e : original)
        s(e);
};

Source<std::string> stringListInput = [](Sink<std::string> s) {
    const std::vector<std::string> input = {"string1", "string2", "foo", "bar"};
    for (auto e : input)
        s(e);
};

Sink<char> consoleOutput = [](char c) { std::cerr << c; };

Sink<std::string> consoleStringOutput = [](std::string s) { std::cerr << s; };

Sink<std::string> newlinedStringOutput = [](std::string s) {
    consoleStringOutput(s);
    std::cerr << std::endl;
};

Transform<char, char> toAllCaps = [](Sink<char> si, char c) { si(toupper(c)); };

Transform<char, std::string> getLines = [](Sink<std::string> si, char c) {
    static std::string current;
    Sink<char> collectChar = [&](char d) { current.push_back(d); };

    if (c == '\n') {
        si(current);
        current.clear();
    } else
        collectChar(c);
};

Transform<char, std::string> getWords = [](Sink<std::string> si, char c) {
    static std::string current;
    Sink<char> collectChar = [&](char d) { current.push_back(d); };

    if (c == ' ') {
        si(current);
        current.clear();
    } else
        collectChar(c);
};

Transform<std::string, char> wordToChars = [](Sink<char> si, std::string s) {
    for (auto e : s)
        si(e);
    si(' ');
};

// connect source -> sink
// connect source -> transform -> sink // same type
// connect source -> transform -> sink // different type
// connect source -> transform -> transform -> sink // same types
// connect source -> transform -> transform -> sink // differing types
// connect source -> transform -> transform -> sink // use named sub-streams

}  // namespace StreamTests

TEST(Stream_Stream, input_to_console)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::stringInput >> StreamTests::consoleOutput;

    EXPECT_EQ("Test String ", s.str());
}

TEST(Stream_Stream, char_input_to_all_caps)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::stringInput >> StreamTests::toAllCaps >> StreamTests::consoleOutput;

    EXPECT_EQ("TEST STRING ", s.str());
}

TEST(Stream_Stream, stringlist_to_chars)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::stringListInput >> StreamTests::wordToChars >> StreamTests::consoleOutput;

    EXPECT_EQ("string1 string2 foo bar ", s.str());
}

TEST(Stream_Stream, char_input_split_into_words_and_printed_to_console)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::stringInput >> StreamTests::getWords >> StreamTests::newlinedStringOutput;

    EXPECT_EQ("Test\nString\n", s.str());
}

TEST(Stream_Stream, newlined_input_printed_to_console)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::newlinedStringInput >> StreamTests::getLines >> StreamTests::newlinedStringOutput;

    EXPECT_EQ("Some\nfile\nwith\nnewlines\n", s.str());
}

TEST(Stream_Stream, composing_transform_chains)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::newlinedStringInput >> StreamTests::getLines >> StreamTests::wordToChars >>
        StreamTests::consoleOutput;

    EXPECT_EQ("Some file with newlines ", s.str());
}

TEST(Stream_Stream, composing_named_transform_chains)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    Source<std::string> inputLines = StreamTests::newlinedStringInput >> StreamTests::getLines;
    Sink<std::string> wordOutput = StreamTests::wordToChars >> StreamTests::consoleOutput;
    inputLines >> wordOutput;

    EXPECT_EQ("Some file with newlines ", s.str());
}

namespace StreamTests {

Source<int> intList = [](Sink<int> out) {
    const std::vector<int> ints = {1, 2, 3, 4, 5};
    for (auto i : ints)
        out(i);
};

Sink<int> intConsoleOutput = [](int i) { std::cerr << i; };

Sink<int> spacedIntConsoleOutput = [](int i) {
    intConsoleOutput(i);
    std::cerr << " ";
};

Sink<double> doubleConsoleOutput = [](double d) { std::cerr << d; };

Sink<double> spacedDoubleConsoleOutput = [](double d) {
    doubleConsoleOutput(d);
    std::cerr << " ";
};

Transform<int, int> tripled = [](Sink<int> out, int i) { out(i * 3); };

Transform<int, double> squareRoot = [](Sink<double> out, int i) { out(sqrt(i)); };

Transform<double, std::string> addCookies = [](Sink<std::string> out, double d) {
    const std::string result = std::to_string(d) + "cookies";
    out(result);
};

struct MyClass
{
    static void Increment(const Sink<int>& out, const int& i) { out(i + 1); }
};

}  // namespace StreamTests

TEST(Stream_Stream, arithmetic_stream_to_console)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::intList >> StreamTests::spacedIntConsoleOutput;

    EXPECT_EQ("1 2 3 4 5 ", s.str());
}

TEST(Stream_Stream, arithmetic_transformed_stream_to_console)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::intList >> StreamTests::tripled >> StreamTests::spacedIntConsoleOutput;

    EXPECT_EQ("3 6 9 12 15 ", s.str());
}

TEST(Stream_Stream, arithmetic_transformed_to_different_type_stream_to_console)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::intList >> StreamTests::tripled >> StreamTests::squareRoot >>
        StreamTests::spacedDoubleConsoleOutput;

    EXPECT_EQ("1.73205 2.44949 3 3.4641 3.87298 ", s.str());
}

TEST(Stream_Stream, append_text_to_arithmetic_stream)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::intList >> StreamTests::tripled >> StreamTests::squareRoot >>
        StreamTests::addCookies >> StreamTests::newlinedStringOutput;

    EXPECT_EQ(
        "1.732051cookies\n"
        "2.449490cookies\n"
        "3.000000cookies\n"
        "3.464102cookies\n"
        "3.872983cookies\n",
        s.str());
}

TEST(Stream_Stream, append_text_to_arithmetic_stream_then_modify)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::intList >> StreamTests::tripled  // x * 3
        >> StreamTests::squareRoot                // sqrt(x)
        >> StreamTests::addCookies                // string result = to_string(x) + "cookies"
        >> StreamTests::wordToChars               // converts strings to chars
        >> StreamTests::toAllCaps                 // toupper(c)
        >> StreamTests::getWords                  // chars back to strings, split on delim = ' '
        >> StreamTests::newlinedStringOutput;     // cerr << s << endl

    EXPECT_EQ(
        "1.732051COOKIES\n"
        "2.449490COOKIES\n"
        "3.000000COOKIES\n"
        "3.464102COOKIES\n"
        "3.872983COOKIES\n",
        s.str());
}

TEST(Stream_Stream, stream_works_with_static_public_member_function)
{
    std::ostringstream s;
    tests::CerrRedirect redirect(s.rdbuf());

    StreamTests::intList >> Transform<int, int>(&StreamTests::MyClass::Increment) >>
        StreamTests::tripled                   // x * 3
        >> StreamTests::squareRoot             // sqrt(x)
        >> StreamTests::addCookies             // string result = to_string(x) + "cookies"
        >> StreamTests::wordToChars            // converts strings to chars
        >> StreamTests::toAllCaps              // toupper(c)
        >> StreamTests::getWords               // chars back to strings, split on delim = ' '
        >> StreamTests::newlinedStringOutput;  // cerr << s << endl

    EXPECT_EQ(
        "2.449490COOKIES\n"
        "3.000000COOKIES\n"
        "3.464102COOKIES\n"
        "3.872983COOKIES\n"
        "4.242641COOKIES\n",
        s.str());
}

#include <pbcopper/utility/Overloaded.h>

#include <pbcopper/utility/Ssize.h>

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <variant>
#include <vector>

using namespace PacBio;

// clang-format off
TEST(Utility_Overloaded, can_overload_operations_on_variant_subtypes)
{
    struct Foo{ const int Value = 42; };
    struct Bar{ const int Value = 5; };
    using V = std::variant<int, double, std::string, Foo, Bar>;
    
    const std::vector<V> variants {
        0, "pi", Bar{}, 3.14, Foo{}
    };
    
    std::ostringstream out;
    for (const auto& v : variants) {
        std::visit(Utility::Overloaded{
            [&](const int x)          { out << "int:" << x; },
            [&](const double d)       { out << "double:" << d; },
            [&](const std::string& s) { out << "string:" << s; },
            [&](const auto& x)        { out << "default:" << x.Value; }
        }, v);
        out << ", ";
    }  
    const std::string expected{
        "int:0, string:pi, default:5, double:3.14, default:42, "
    };
    EXPECT_EQ(out.str(), expected);

    int count = 0;
    for (const auto& v : variants) {
        count += std::visit(Utility::Overloaded{
                    [](const int x)                 { return x; },
                    [](const double d) -> int       { return d; },
                    [](const std::string& s) -> int { return Utility::Ssize(s); },
                    [](const auto& x)               { return x.Value; }
                }, v);
    }
    EXPECT_EQ(count, 52);  // 0 + 2 + 5 + 3 + 42
}
// clang-format on

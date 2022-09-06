#ifndef PBCOPPER_UTILITY_OVERLOADED_H
#define PBCOPPER_UTILITY_OVERLOADED_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

//
// std::variant convenience utility, which dispatches handling of variant subtypes
// to wrapped lambdas
//
// See description: https://en.cppreference.com/w/cpp/utility/variant/visit
//
// std::variant<int, double, std::string, std::vector<int>, Foo> value;
// std::visit(Utility::Overloaded{
//    [](const int x) { ... handle as int ... },
//    [](const double x) { ... handle as double ... },
//    [](const std::string& x) { ... handle as string ... },
//    [](const auto& x) { ... fallback for other types ... }
// }, value);
//

template <class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_OVERLOADED_H

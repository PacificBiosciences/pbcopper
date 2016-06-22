#ifndef PBCOPPER_CLI_RESULTS_INL_H
#define PBCOPPER_CLI_RESULTS_INL_H

#include "pbcopper/cli/Results.h"

namespace PacBio {
namespace CLI {

//Result& Results::operator[](const std::string& optionId) const
//{ return FetchResult(optionId); }

//template<>
//inline std::string Results::operator[](const std::string& optionName) const
//{
////    return string{};
//    const auto values = Values(optionName);
//    if (!values.empty())
//        return values.back();
//    return std::string(); // or throw on unknown name?
//}

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_RESULTS_INL_H

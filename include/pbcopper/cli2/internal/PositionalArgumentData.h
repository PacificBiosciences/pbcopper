// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_POSITONALARGUMENTDATA_H
#define PBCOPPER_CLI_v2_POSITONALARGUMENTDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/cli2/OptionValue.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for interface positional arguments
///
struct PositionalArgumentData
{
    std::string name;
    std::string description;
    bool required = true;

    OptionValueType type = OptionValueType::STRING;

    bool operator==(const PositionalArgumentData& other) const noexcept
    {
        return name == other.name;
    }
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

namespace std {

template <>
struct hash<PacBio::CLI_v2::internal::PositionalArgumentData>
{
    size_t operator()(const PacBio::CLI_v2::internal::PositionalArgumentData& k) const noexcept
    {
        return std::hash<std::string>()(k.name);
    }
};

}  // namespace std

#endif  // PBCOPPER_CLI_v2_POSITONALARGUMENTDATA_H

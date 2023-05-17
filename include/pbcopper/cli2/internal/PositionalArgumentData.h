#ifndef PBCOPPER_CLI_v2_POSITONALARGUMENTDATA_H
#define PBCOPPER_CLI_v2_POSITONALARGUMENTDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/OptionValue.h>

#include <optional>
#include <string>
#include <vector>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for interface positional arguments
///
struct PositionalArgumentData
{
    std::string Name;
    std::string Description;
    bool Required = true;

    OptionValueType Type = OptionValueType::STRING;

    bool operator==(const PositionalArgumentData& other) const noexcept
    {
        return Name == other.Name;
    }
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

namespace std {

template <>
struct hash<PacBio::CLI_v2::internal::PositionalArgumentData>
{
    std::size_t operator()(const PacBio::CLI_v2::internal::PositionalArgumentData& k) const noexcept
    {
        return std::hash<std::string>()(k.Name);
    }
};

}  // namespace std

#endif  // PBCOPPER_CLI_v2_POSITONALARGUMENTDATA_H

#ifndef PBCOPPER_CLI_v2_OPTIONDATA_H
#define PBCOPPER_CLI_v2_OPTIONDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/OptionValue.h>

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for interface options
///
struct OptionData
{
    std::vector<std::string> Names;
    std::vector<std::string> HiddenNames;
    std::string Description;

    OptionValueType Type = OptionValueType::BOOL;
    std::optional<OptionValue> DefaultValue;
    bool IsRequired = false;
    bool IsHidden = false;
    bool IsDefaultValueHidden = false;

    std::vector<OptionValue> Choices;
    bool IsChoicesHidden = false;

    bool operator==(const OptionData& other) const noexcept { return Names == other.Names; }
};

///
/// Named group of options
///
struct OptionGroupData
{
    OptionGroupData() = default;

    OptionGroupData(std::string n, std::vector<OptionData> opts)
        : Name{std::move(n)}, Options{std::move(opts)}
    {}

    std::string Name;
    std::vector<OptionData> Options;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

namespace std {

template <>
struct hash<PacBio::CLI_v2::internal::OptionData>
{
    std::size_t operator()(const PacBio::CLI_v2::internal::OptionData& k) const noexcept
    {
        return std::hash<std::string>()(k.Names[0]);
    }
};

}  // namespace std

#endif  // PBCOPPER_CLI_v2_OPTIONDATA_H

#include <pbcopper/cli2/MultiToolInterface.h>

#include <cassert>
#include <type_traits>

#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/InterfaceData.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>

using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;

namespace PacBio {
namespace CLI_v2 {

static_assert(std::is_copy_constructible<MultiToolInterface>::value,
              "MultiToolInterface(const MultiToolInterface&) is not = default");
static_assert(std::is_copy_assignable<MultiToolInterface>::value,
              "MultiToolInterface& operator=(const MultiToolInterface&) is not = default");

static_assert(std::is_nothrow_move_constructible<MultiToolInterface>::value,
              "MultiToolInterface(MultiToolInterface&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<MultiToolInterface>::value ==
                  std::is_nothrow_move_assignable<internal::MultiToolInterfaceData>::value,
              "");

MultiToolInterface::MultiToolInterface(std::string name, std::string description,
                                       std::string version)
    : data_{std::move(name), std::move(description), std::move(version),
            OptionTranslator::Translate(Builtin::Help),
            OptionTranslator::Translate(Builtin::Version)}
{
}

MultiToolInterface& MultiToolInterface::AddTool(const Tool& tool)
{
    data_.tools_.emplace_back(tool);
    return *this;
}

MultiToolInterface& MultiToolInterface::AddTools(const std::vector<Tool>& tools)
{
    for (const auto& tool : tools)
        AddTool(tool);
    return *this;
}

const std::string& MultiToolInterface::ApplicationDescription() const
{
    return data_.appDescription_;
}

const std::string& MultiToolInterface::ApplicationName() const { return data_.appName_; }

const std::string& MultiToolInterface::ApplicationVersion() const { return data_.appVersion_; }

bool MultiToolInterface::HasTool(const std::string& toolName) const
{
    for (const auto& tool : data_.tools_) {
        if (tool.name == toolName) return true;
    }
    return false;
}

const std::string& MultiToolInterface::HelpFooter() const { return data_.helpFooter_; }

MultiToolInterface& MultiToolInterface::HelpFooter(std::string footer)
{
    data_.helpFooter_ = std::move(footer);
    return *this;
}

const internal::OptionData& MultiToolInterface::HelpOption() const { return data_.helpOption_; }

const Tool& MultiToolInterface::ToolFor(const std::string& toolName) const
{
    for (const auto& tool : data_.tools_) {
        if (tool.name == toolName) return tool;
    }
    throw std::runtime_error{"[pbcopper] command line ERROR: unknown tool '" + toolName +
                             "' requested"};
}

const std::vector<Tool>& MultiToolInterface::Tools() const { return data_.tools_; }

const internal::OptionData& MultiToolInterface::VersionOption() const
{
    return data_.versionOption_;
}

}  // namespace CLI_v2
}  // namespace PacBio

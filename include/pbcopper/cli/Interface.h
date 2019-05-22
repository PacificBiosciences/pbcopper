// Author: Derek Barnett

#ifndef PBCOPPER_CLI_INTERFACE_H
#define PBCOPPER_CLI_INTERFACE_H

#include <memory>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/Option.h>
#include <pbcopper/cli/PositionalArg.h>
#include <pbcopper/cli/SingleDashMode.h>
#include <pbcopper/cli/toolcontract/Config.h>

namespace PacBio {
namespace CLI {

///
/// \brief The Interface class
///
class Interface
{
public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief Interface
    /// \param appName
    /// \param appDescription
    /// \param appVersion
    ///
    Interface(std::string appName, std::string appDescription = std::string(),
              std::string appVersion = std::string());

    ///
    /// \brief Interface
    /// \param other
    ///
    Interface(const Interface&);
    Interface(Interface&&) noexcept;
    Interface& operator=(const Interface&);
    Interface& operator=(Interface&&) noexcept;
    ~Interface();

    /// \}

public:
    /// \name Application Attributes
    /// \{

    ///
    /// \brief ApplicationDescription
    /// \return
    ///
    const std::string& ApplicationDescription() const;

    ///
    /// \brief ApplicationName
    /// \return
    ///
    const std::string& ApplicationName() const;

    ///
    /// \brief ApplicationVersion
    /// \return
    ///
    const std::string& ApplicationVersion() const;

    ///
    /// \brief AlternativeToolContractName
    /// \return
    ///
    const std::string& AlternativeToolContractName() const;

    /// \}

public:
    /// \name Interface Query
    /// \{

    ///
    /// \brief ExpectsValue
    /// \param optionName
    /// \return
    ///
    bool ExpectsValue(const std::string& optionName) const;

    ///
    /// \brief Groups
    /// \return
    ///
    const std::vector<std::string>& Groups() const;

    ///
    /// \brief GroupOptions
    /// \param name
    /// \return
    ///
    std::vector<Option> GroupOptions(const std::string& group) const;

    ///
    /// \brief HasOptionRegistered
    /// \param optionName
    /// \return
    ///
    bool HasOptionRegistered(const std::string& optionName) const;

    ///
    /// \brief HasHelpOptionRegistered
    /// \return
    ///
    bool HasHelpOptionRegistered() const;

    ///
    /// \brief HasLogLevelOptionRegistered
    /// \return
    ///
    bool HasLogLevelOptionRegistered() const;

    ///
    /// \brief HasVerboseOptionRegistered
    /// \return
    ///
    bool HasVerboseOptionRegistered() const;

    ///
    /// \brief HasVersionOptionRegistered
    /// \return
    ///
    bool HasVersionOptionRegistered() const;

    ///
    /// \brief HelpOption
    /// \return
    ///
    const Option& HelpOption() const;

    ///
    /// \brief IdForOptionName
    /// \param optionName
    /// \return
    ///
    std::string IdForOptionName(const std::string& optionName) const;

    ///
    /// \brief IsToolContractEnabled
    /// \return
    ///
    bool IsToolContractEnabled() const;

    ///
    /// \brief LogLevelOption
    /// \return
    ///
    const Option& LogLevelOption() const;

    ///
    /// \brief OptionChoices
    /// \param optionId
    /// \return
    ///
    PacBio::JSON::Json OptionChoices(const std::string& optionId) const;

    ///
    /// \brief RegisteredOptions
    /// \return
    ///
    const std::vector<Option>& RegisteredOptions() const;

    ///
    /// \brief RegisteredPositionalArgs
    /// \return
    ///
    const std::vector<PositionalArg>& RegisteredPositionalArgs() const;

    ///
    /// \brief ToolContract
    /// \return
    ///
    const ToolContract::Config& ToolContract() const;

    ///
    /// \brief VerboseOption
    /// \return
    ///
    const Option& VerboseOption() const;

    ///
    /// \brief VersionOption
    /// \return
    ///
    const Option& VersionOption() const;

    /// \}

public:
    /// \name Application Attributes
    /// \{

    ///
    /// \brief ApplicationDescription
    /// \param description
    /// \return
    ///
    Interface& ApplicationDescription(std::string description);

    ///
    /// \brief ApplicationName
    /// \param name
    /// \return
    ///
    Interface& ApplicationName(std::string name);

    ///
    /// \brief ApplicationVersion
    /// \param version
    /// \return
    ///
    Interface& ApplicationVersion(std::string version);

    ///
    /// \brief AlternativeToolContractName
    /// This name, if set, will be used as the alternative prefix of the
    /// tool contract option prefix and replace the actual ApplicationName.
    ///
    /// \param name
    /// \return
    ///
    Interface& AlternativeToolContractName(std::string name);

    /// \}

public:
    /// \name Interface Setup
    /// \{

    ///
    /// \brief AddGroup
    /// \param name
    /// \param option
    /// \return
    ///
    Interface& AddGroup(const std::string& name, const std::vector<Option>& option);

    ///
    /// \brief AddOption
    /// \param option
    /// \return
    ///
    Interface& AddOption(const Option& option);

    ///
    /// \brief AddOptions
    /// \param options
    /// \return
    ///
    Interface& AddOptions(const std::vector<Option>& options);

    ///
    /// \brief AddPositionalArgument
    /// \param posArg
    /// \return
    ///
    Interface& AddPositionalArgument(const PositionalArg& posArg);

    ///
    /// \brief AddPositionalArguments
    /// \param posArgs
    /// \return
    ///
    Interface& AddPositionalArguments(const std::vector<PositionalArg>& posArgs);

    ///
    /// \brief AddHelpOption
    /// \param option
    /// \return
    ///
    Interface& AddHelpOption(const Option& option = Option::DefaultHelpOption());

    ///
    /// \brief AddLogLevelOption
    /// \param option
    /// \return
    ///
    Interface& AddLogLevelOption(const Option& option = Option::DefaultLogLevelOption());

    ///
    /// \brief AddVerboseOption
    /// \param option
    /// \return
    ///
    Interface& AddVerboseOption(const Option& option = Option::DefaultVerboseOption());

    ///
    /// \brief AddVersionOption
    /// \param option
    /// \return
    ///
    Interface& AddVersionOption(const Option& option = Option::DefaultVersionOption());

    ///
    /// \brief EnableToolContract
    /// \param tcConfig
    /// \return
    ///
    Interface& EnableToolContract(const ToolContract::Config& tcConfig);

    /// \}

private:
    class InterfacePrivate;
    std::unique_ptr<InterfacePrivate> d_;
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_INTERFACE_H

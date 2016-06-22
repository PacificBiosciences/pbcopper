#ifndef PBCOPPER_CLI_INTERFACE_H
#define PBCOPPER_CLI_INTERFACE_H

#include "pbcopper/Config.h"
#include "pbcopper/cli/BuiltInOption.h"
#include "pbcopper/cli/Option.h"
#include "pbcopper/cli/PositionalArg.h"
#include "pbcopper/cli/SingleDashMode.h"
#include "pbcopper/cli/toolcontract/Config.h"
#include <memory>
#include <string>
#include <vector>

namespace PacBio {
namespace CLI {

namespace internal { class InterfacePrivate; }

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
    Interface(const std::string& appName,
              const std::string& appDescription = std::string(),
              const std::string& appVersion = std::string());

    ///
    /// \brief Interface
    /// \param other
    ///
    Interface(const Interface& other);

    ~Interface(void);

    /// \}

public:
    /// \name Application Attributes
    /// \{

    ///
    /// \brief ApplicationDescription
    /// \return
    ///
    std::string ApplicationDescription(void) const;

    ///
    /// \brief ApplicationName
    /// \return
    ///
    std::string ApplicationName(void) const;

    ///
    /// \brief ApplicationVersion
    /// \return
    ///
    std::string ApplicationVersion(void) const;

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
    /// \brief HasOptionRegistered
    /// \param optionName
    /// \return
    ///
    bool HasOptionRegistered(const std::string& optionName) const;

    ///
    /// \brief IsBuiltInOptionEnabled
    /// \param type
    /// \return
    ///
    bool IsBuiltInOptionEnabled(const BuiltInOption& type) const;

    ///
    /// \brief RegisteredOptions
    /// \return
    ///
    std::vector<Option> RegisteredOptions(void) const;

    ///
    /// \brief RegisteredPositionalArgs
    /// \return
    ///
    std::vector<PositionalArg> RegisteredPositionalArgs(void) const;

    const ToolContract::Config& ToolContract(void) const;

    std::string IdForOptionName(const std::string& optionName) const;

    /// \}

public:
    /// \name Application Attributes
    /// \{

    ///
    /// \brief ApplicationDescription
    /// \param description
    /// \return
    ///
    Interface& ApplicationDescription(const std::string& description);

    ///
    /// \brief ApplicationName
    /// \param name
    /// \return
    ///
    Interface& ApplicationName(const std::string& name);

    ///
    /// \brief ApplicationVersion
    /// \param version
    /// \return
    ///
    Interface& ApplicationVersion(const std::string& version);

    /// \}

public:
    /// \name Interface Setup
    /// \{
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

    Interface& AddHelpOption(void);
    Interface& AddVerboseOption(void);
    Interface& AddVersionOption(void);

    Interface& EnableToolContract(const ToolContract::Config& tcConfig);

    /// \}

private:
    std::unique_ptr<internal::InterfacePrivate> d_;
};

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_INTERFACE_H

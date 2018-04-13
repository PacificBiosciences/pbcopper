// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_H
#define PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_H

#include <string>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

class InputFileType
{
public:
    InputFileType(const std::string& id, const std::string& title, const std::string& description,
                  const std::string& type);

    InputFileType(const InputFileType& other) = default;
    ~InputFileType(void) = default;

public:
    const std::string& Id(void) const;
    const std::string& Title(void) const;
    const std::string& Description(void) const;
    const std::string& Type(void) const;

private:
    std::string id_;
    std::string title_;
    std::string description_;
    std::string type_;
};

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#include <pbcopper/cli/toolcontract/internal/InputFileType-inl.h>

#endif  // PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_H

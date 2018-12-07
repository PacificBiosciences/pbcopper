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
    InputFileType(std::string id, std::string title, std::string description, std::string type);

    InputFileType(const InputFileType&) = default;
    ~InputFileType() = default;

public:
    const std::string& Id() const;
    const std::string& Title() const;
    const std::string& Description() const;
    const std::string& Type() const;

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

// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_INL_H

#include <pbcopper/cli/toolcontract/InputFileType.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline InputFileType::InputFileType(std::string id, std::string title, std::string description,
                                    std::string type)
    : id_{std::move(id)}
    , title_{std::move(title)}
    , description_{std::move(description)}
    , type_{std::move(type)}
{
}

inline const std::string& InputFileType::Description(void) const { return description_; }

inline const std::string& InputFileType::Id(void) const { return id_; }

inline const std::string& InputFileType::Title(void) const { return title_; }

inline const std::string& InputFileType::Type(void) const { return type_; }

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_INL_H

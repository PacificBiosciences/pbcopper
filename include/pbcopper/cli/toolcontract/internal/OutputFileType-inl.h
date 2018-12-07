// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H

#include <pbcopper/cli/toolcontract/OutputFileType.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline OutputFileType::OutputFileType(std::string id, std::string title, std::string description,
                                      std::string type, std::string defaultName)
    : id_{std::move(id)}
    , title_{std::move(title)}
    , description_{std::move(description)}
    , type_{std::move(type)}
    , defaultName_{std::move(defaultName)}
{
}

inline const std::string& OutputFileType::DefaultName() const { return defaultName_; }

inline const std::string& OutputFileType::Description() const { return description_; }

inline const std::string& OutputFileType::Id() const { return id_; }

inline const std::string& OutputFileType::Title() const { return title_; }

inline const std::string& OutputFileType::Type() const { return type_; }

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H

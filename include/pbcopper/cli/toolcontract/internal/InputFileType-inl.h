#ifndef PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_INL_H

#include "pbcopper/cli/toolcontract/InputFileType.h"

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline InputFileType::InputFileType(const std::string& id,
                                    const std::string& title,
                                    const std::string& description,
                                    const std::string& type)
    : id_(id)
    , title_(title)
    , description_(description)
    , type_(type)
{ }

inline const std::string& InputFileType::Description(void) const
{ return description_; }

inline const std::string& InputFileType::Id(void) const
{ return id_; }

inline const std::string& InputFileType::Title(void) const
{ return title_; }

inline const std::string& InputFileType::Type(void) const
{ return type_; }

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio


#endif // PBCOPPER_CLI_TOOLCONTRACT_INPUTFILETYPE_INL_H

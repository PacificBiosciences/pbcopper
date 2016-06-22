#ifndef PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H

#include "pbcopper/cli/toolcontract/OutputFileType.h"

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline OutputFileType::OutputFileType(const std::string& id,
                                      const std::string& title,
                                      const std::string& description,
                                      const std::string& type,
                                      const std::string& defaultName)
    : id_(id)
    , title_(title)
    , description_(description)
    , type_(type)
    , defaultName_(defaultName)
{ }

inline const std::string& OutputFileType::DefaultName(void) const
{ return defaultName_; }

inline const std::string& OutputFileType::Description(void) const
{ return description_; }

inline const std::string& OutputFileType::Id(void) const
{ return id_; }

inline const std::string& OutputFileType::Title(void) const
{ return title_; }

inline const std::string& OutputFileType::Type(void) const
{ return type_; }

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio


#endif // PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H

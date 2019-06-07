// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_INL_H

#include <pbcopper/cli/toolcontract/OutputFileType.h>

#include <cassert>
#include <type_traits>

namespace PacBio {
namespace CLI {
namespace ToolContract {

static_assert(std::is_copy_constructible<OutputFileType>::value,
              "OutputFileType(const OutputFileType&) is not = default");
static_assert(std::is_copy_assignable<OutputFileType>::value,
              "OutputFileType& operator=(const OutputFileType&) is not = default");

static_assert(std::is_nothrow_move_constructible<OutputFileType>::value,
              "OutputFileType(OutputFileType&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<OutputFileType>::value ==
                  std::is_nothrow_move_assignable<std::string>::value,
              "");

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

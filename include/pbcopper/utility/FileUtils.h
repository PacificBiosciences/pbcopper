#ifndef PBCOPPER_UTILITY_FILEUTILS_H
#define PBCOPPER_UTILITY_FILEUTILS_H

#include <string>

namespace PacBio {
namespace Utility {

///FilteUtils provides methods to manipulate file paths

bool FileExists(const std::string& path);

std::string FileExtension(const std::string& path);

std::string FilePrefix(const std::string& path);

} // namespace Utility
} // namespace PacBio

#include "internal/FileUtils-inl.h"

#endif // PBCOPPER_UTILITY_FILEUTILS_H

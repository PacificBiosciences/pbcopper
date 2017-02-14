#ifndef PBCOPPER_UTILITY_FILEUTILS_INL_H
#define PBCOPPER_UTILITY_FILEUTILS_INL_H

#include "pbcopper/utility/FileUtils.h"

#include <climits>
#include <cstdlib>
#include <fstream>

#include <sys/stat.h>

#include <boost/algorithm/string.hpp>

namespace PacBio {
namespace Utility {

inline bool FileExists(const std::string& path)
{
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

inline std::string FileExtension(const std::string& path)
{
    size_t fileStart = path.find_last_of("/");

    if (fileStart == std::string::npos) fileStart = 0;

    // increment beyond the '/'
    ++fileStart;

    size_t extStart = path.substr(fileStart, path.length() - fileStart).find_last_of(".");

    if (extStart == std::string::npos) return "";

    // increment beyond '.'
    ++extStart;

    auto suffix = path.substr(fileStart + extStart, path.length() - fileStart - extStart);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
    return suffix;
}

inline std::string FilePrefix(const std::string& path)
{
    size_t fileStart = path.find_last_of("/");

    if (fileStart == std::string::npos) fileStart = -1;

    // increment beyond the '/'
    ++fileStart;

    size_t extStart = path.substr(fileStart, path.length() - fileStart).find_first_of(".");

    if (extStart == std::string::npos) return "";

    auto suffix = path.substr(fileStart, extStart);
    return suffix;
}

} // namespace Utility
} // namespace PacBio


#endif // PBCOPPER_UTILITY_FILEUTILS_INL_H

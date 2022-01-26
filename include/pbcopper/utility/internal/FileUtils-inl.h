#ifndef PBCOPPER_UTILITY_FILEUTILS_INL_H
#define PBCOPPER_UTILITY_FILEUTILS_INL_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/utility/FileUtils.h>

#include <boost/algorithm/string.hpp>

#include <climits>
#include <cstddef>
#include <cstdlib>

#include <sys/stat.h>

namespace PacBio {
namespace Utility {

inline bool FileExists(const std::string& path)
{
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

inline std::string FileExtension(const std::string& path)
{
    const auto lastDot = path.rfind('.');
    if (lastDot == std::string::npos) {
        return std::string{};
    }
    return path.substr(lastDot + 1);
}

inline std::string FilePrefix(const std::string& path)
{
    const auto lastSlash = path.rfind('/');
    const auto filename = (lastSlash == std::string::npos ? path : path.substr(lastSlash + 1));
    const auto firstDot = filename.find('.');
    return filename.substr(0, firstDot);
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_FILEUTILS_INL_H

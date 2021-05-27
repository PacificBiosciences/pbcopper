#ifndef PBCOPPER_LIBRARY_INFO_H
#define PBCOPPER_LIBRARY_INFO_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

namespace PacBio {
namespace Library {

struct Info
{
    std::string Name;
    std::string Release;
    std::string GitSha1;
};

inline std::string FormattedVersion(const Library::Info& library)
{
    std::string result = library.Release;
    if (!library.GitSha1.empty()) {
        result += " (commit " + library.GitSha1 + ")";
    }
    return result;
}

}  // namespace Library
}  // namespace PacBio

#endif  //PBCOPPER_LIBRARY_INFO_H

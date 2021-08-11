#ifndef PBCOPPER_UTILITY_BASE64_H
#define PBCOPPER_UTILITY_BASE64_H

#include <string>

namespace PacBio {
namespace Utility {

struct Base64
{
    ///
    /// \return original data from Base64 encoding
    ///
    static std::string Decode(const std::string s);

    ///
    /// \return Base64 encoding of input data
    ///
    static std::string Encode(const std::string& s);
};

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_BASE64_H

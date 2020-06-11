// Author: Derek Barnett

#include <pbcopper/utility/Base64.h>

#include "third-party/base64/base64.h"

namespace PacBio {
namespace Utility {

std::string Base64::Decode(const std::string s) { return base64_decode(s); }

std::string Base64::Encode(const std::string& s) { return base64_encode(s); }

}  // namespace Utility
}  // namespace PacBio

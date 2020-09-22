#include <pbcopper/utility/StringView.h>

#include <ostream>

namespace PacBio {
namespace Utility {

std::ostream& operator<<(std::ostream& os, const StringView str)
{
    return os.write(str.Data(), str.Size());
}

}  // namespace Utility
}  // namespace PacBio

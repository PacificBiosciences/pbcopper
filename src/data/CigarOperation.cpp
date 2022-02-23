#include <pbcopper/data/CigarOperation.h>

#include <string_view>

#include <cassert>

namespace PacBio {
namespace Data {

bool CigarOperation::AutoValidateCigar = true;

char CigarOperation::Char() const { return CigarOperation::TypeToChar(Type()); }

void CigarOperation::DisableAutoValidation() { AutoValidateCigar = false; }

char CigarOperation::TypeToChar(const CigarOperationType type)
{
    constexpr std::string_view lookupTable{"MIDNSHP=XB??????"};
    return lookupTable[static_cast<int32_t>(type)];
}

}  // namespace Data
}  // namespace PacBio

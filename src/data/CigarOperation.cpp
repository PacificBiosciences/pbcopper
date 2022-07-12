#include <pbcopper/data/CigarOperation.h>

#include <string_view>

#include <cassert>

namespace PacBio {
namespace Data {

char CigarOperation::Char() const { return CigarOperation::TypeToChar(Type()); }

void CigarOperation::EnableAutoValidation() { AutoValidateCigar = true; }

void CigarOperation::DisableAutoValidation() { AutoValidateCigar = false; }

char CigarOperation::TypeToChar(const CigarOperationType type)
{
    constexpr std::string_view LOOKUP_TABLE{"MIDNSHP=XB??????"};
    return LOOKUP_TABLE[static_cast<int32_t>(type)];
}

bool ConsumesQuery(const CigarOperationType type) noexcept
{
    //                                X=PHSNDIM
    constexpr uint32_t LOOKUP_TABLE{0b110010011};
    const auto val = static_cast<uint32_t>(type);
    assert(val <= 8);

    return (LOOKUP_TABLE >> val) & 0b1U;
}

bool ConsumesReference(const CigarOperationType type) noexcept
{
    //                                X=PHSNDIM
    constexpr uint32_t LOOKUP_TABLE{0b110001101};
    const auto val = static_cast<uint32_t>(type);
    assert(val <= 8);

    return (LOOKUP_TABLE >> val) & 0b1U;
}

}  // namespace Data
}  // namespace PacBio

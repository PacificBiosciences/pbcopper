// Author: Derek Barnett

#include <pbcopper/data/CigarOperation.h>

#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace PacBio {
namespace Data {

CigarOperationType CigarOperation::CharToType(const char c)
{
    // clang-format off
    static const auto lookup = std::unordered_map<char, CigarOperationType>
    {
        { 'S', CigarOperationType::SOFT_CLIP },
        { '=', CigarOperationType::SEQUENCE_MATCH },
        { 'X', CigarOperationType::SEQUENCE_MISMATCH },
        { 'I', CigarOperationType::INSERTION },
        { 'D', CigarOperationType::DELETION },
        { 'N', CigarOperationType::REFERENCE_SKIP },
        { 'H', CigarOperationType::HARD_CLIP },
        { 'P', CigarOperationType::PADDING },
        { 'M', CigarOperationType::ALIGNMENT_MATCH }
    };
    // clang-format on

    const auto found = lookup.find(c);
    if (found == lookup.cend()) {
        std::ostringstream s;
        s << "pbcopper: unrecognized CIGAR char code " << c << " (int: " << static_cast<int>(c)
          << ")";
        throw std::runtime_error(s.str());
    } else
        return found->second;
}

char CigarOperation::TypeToChar(const CigarOperationType type)
{
    // clang-format off
    static const auto lookup = std::unordered_map<CigarOperationType, char>
    {
        { CigarOperationType::SOFT_CLIP,         'S' },
        { CigarOperationType::SEQUENCE_MATCH,    '=' },
        { CigarOperationType::SEQUENCE_MISMATCH, 'X' },
        { CigarOperationType::INSERTION,         'I' },
        { CigarOperationType::DELETION,          'D' },
        { CigarOperationType::REFERENCE_SKIP,    'N' },
        { CigarOperationType::HARD_CLIP,         'H' },
        { CigarOperationType::PADDING,           'P' },
        { CigarOperationType::ALIGNMENT_MATCH,   'M' }
    };
    // clang-format on

    const auto found = lookup.find(type);
    if (found == lookup.cend()) {
        std::ostringstream s;
        s << "pbcopper: unrecognized CIGAR operation value " << static_cast<int>(type);
        throw std::runtime_error(s.str());
    } else
        return found->second;
}

std::istream& operator>>(std::istream& in, CigarOperation& op)
{
    uint32_t l;
    char c;

    in >> l >> c;

    op.Length(l);
    op.Char(c);
    return in;
}

std::ostream& operator<<(std::ostream& out, const CigarOperation& op)
{
    out << op.Length() << op.Char();
    return out;
}

}  // namespace Data
}  // namespace PacBio

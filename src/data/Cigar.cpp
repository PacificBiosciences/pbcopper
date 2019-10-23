// Author: Derek Barnett

#include <pbcopper/data/Cigar.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <type_traits>

namespace PacBio {
namespace Data {

static_assert(std::is_copy_constructible<Cigar>::value, "Cigar(const Cigar&) is not = default");
static_assert(std::is_copy_assignable<Cigar>::value,
              "Cigar& operator=(const Cigar&) is not = default");

#ifndef __INTEL_COMPILER
static_assert(std::is_nothrow_move_constructible<Cigar>::value, "Cigar(Cigar&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Cigar>::value,
              "Cigar& operator=(Cigar&&) is not = noexcept");
#endif

Cigar::Cigar(const char* str) : std::vector<CigarOperation>{}
{
    size_t numberStart = 0;
    size_t i = 0;
    for (const char *c = str; *c; ++i, ++c) {
        if (!std::isdigit(*c)) {
            const size_t distance = i - numberStart;
            const uint32_t length = std::stoul(std::string{str, numberStart, distance});
            push_back(CigarOperation(*c, length));
            numberStart = i + 1;
        }
    }
}

Cigar::Cigar(const std::string& cigarString) : Cigar{cigarString.c_str()} {}

Cigar::Cigar(std::vector<CigarOperation> cigar) : std::vector<CigarOperation>{std::move(cigar)} {}

Cigar Cigar::FromStdString(const std::string& stdString) { return Cigar(stdString.c_str()); }

Cigar Cigar::FromCStr(const char* str) { return Cigar(str); }

std::string Cigar::ToStdString() const
{
    std::ostringstream s;
    const auto endIt = this->cend();
    for (auto iter = this->cbegin(); iter != endIt; ++iter) {
        const CigarOperation& cigar = (*iter);
        s << cigar.Length() << cigar.Char();
    }
    return s.str();
}

size_t ReferenceLength(const Cigar& cigar)
{
    size_t length = 0;
    for (const auto& op : cigar) {
        if (ConsumesReference(op.Type())) length += op.Length();
    }
    return length;
}

std::ostream& operator<<(std::ostream& os, const Cigar& cigar)
{
    return os << "Cigar(" << cigar.ToStdString() << ')';
}

}  // namespace Data
}  // namespace PacBio

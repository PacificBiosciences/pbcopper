// Author: Derek Barnett

#include <pbcopper/data/Cigar.h>

#include <cctype>
#include <cstddef>
#include <sstream>

namespace PacBio {
namespace Data {

Cigar::Cigar() = default;

Cigar::Cigar(const std::string& cigarString) : std::vector<CigarOperation>()
{
    size_t numberStart = 0;
    const auto numChars = cigarString.size();
    for (size_t i = 0; i < numChars; ++i) {
        const char c = cigarString.at(i);
        if (!isdigit(c)) {
            const auto distance = i - numberStart;
            const auto length = std::stoul(cigarString.substr(numberStart, distance));
            emplace_back(c, length);
            numberStart = i + 1;
        }
    }
}

std::string Cigar::ToStdString() const
{
    std::ostringstream s;
    for (const auto& op : *this) {
        s << op.Length() << op.Char();
    }
    return s.str();
}

std::istream& operator>>(std::istream& in, Cigar& cigar)
{
    std::string s;
    in >> s;
    cigar = Cigar{s};
    return in;
}

std::ostream& operator<<(std::ostream& out, const Cigar& cigar)
{
    out << cigar.ToStdString();
    return out;
}

}  // namespace Data
}  // namespace PacBio

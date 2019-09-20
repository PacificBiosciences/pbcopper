// Author: Lance Hepler

#include <pbcopper/data/ReadId.h>

#include <iostream>
#include <sstream>
#include <string>

namespace PacBio {
namespace Data {

ReadId::ReadId(const std::string& readName) : ReadName{readName} {}

ReadId::ReadId(const std::string& movieName, size_t holeNumber)
    : MovieName{movieName}, HoleNumber{holeNumber}, ZmwInterval{boost::none}
{
}

ReadId::ReadId(const std::string& movieName, size_t holeNumber, const Interval& interval)
    : MovieName{movieName}, HoleNumber{holeNumber}, ZmwInterval{interval}
{
}

ReadId::ReadId(const std::shared_ptr<std::string>& movieName, size_t holeNumber)
    : ReadId{*movieName, holeNumber}
{
}

ReadId::ReadId(const std::shared_ptr<std::string>& movieName, size_t holeNumber,
               const Interval& interval)
    : ReadId{*movieName, holeNumber, interval}
{
}

ReadId::operator std::string() const
{
    std::ostringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const ReadId& id)
{
    if (id.ReadName.empty()) {
        os << id.MovieName << '/' << id.HoleNumber;
        if (id.ZmwInterval) {
            os << '/' << id.ZmwInterval->Start() << '_' << id.ZmwInterval->End();
        }
    } else {
        os << id.ReadName;
    }

    return os;
}

}  // namespace Data
}  // namespace PacBio

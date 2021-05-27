#ifndef PBCOPPER_DATA_READID_H
#define PBCOPPER_DATA_READID_H

#include <cstddef>

#include <iosfwd>
#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <pbcopper/data/Interval.h>

namespace PacBio {
namespace Data {

struct ReadId
{
    std::string MovieName;
    size_t HoleNumber;
    boost::optional<Interval> ZmwInterval;
    std::string ReadName;

    ReadId(const std::string& readName);

    ReadId(const std::string& movieName, size_t holeNumber);
    ReadId(const std::string& movieName, size_t holeNumber, const Interval& interval);

    ReadId(const std::shared_ptr<std::string>& movieName, size_t holeNumber);
    ReadId(const std::shared_ptr<std::string>& movieName, size_t holeNumber,
           const Interval& interval);

    operator std::string() const;
    friend std::ostream& operator<<(std::ostream&, const ReadId&);
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_READID_H

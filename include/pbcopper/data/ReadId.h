#ifndef PBCOPPER_DATA_READID_H
#define PBCOPPER_DATA_READID_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Interval.h>

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>

#include <cstddef>

namespace PacBio {
namespace Data {

struct ReadId
{
    std::string MovieName;
    std::size_t HoleNumber;
    std::optional<Interval> ZmwInterval;
    std::string ReadName;

    ReadId(const std::string& readName);

    ReadId(const std::string& movieName, std::size_t holeNumber);
    ReadId(const std::string& movieName, std::size_t holeNumber, const Interval& interval);

    ReadId(const std::shared_ptr<std::string>& movieName, std::size_t holeNumber);
    ReadId(const std::shared_ptr<std::string>& movieName, std::size_t holeNumber,
           const Interval& interval);

    operator std::string() const;
    friend std::ostream& operator<<(std::ostream&, const ReadId&);
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_READID_H

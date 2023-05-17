#include <pbcopper/data/GenomicInterval.h>

#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <cctype>
#include <cstdlib>
#include <cstring>

namespace PacBio {
namespace Data {
namespace {

struct RegionStringException : public std::runtime_error
{
    RegionStringException(std::string region)
        : std::runtime_error{"[pbcopper] genomic interval ERROR: malformed region string '" +
                             region + "'"}
    {}
};

// returns sequence name & sets begin/end, from input regionString
std::string parseRegionString(const std::string& reg, Position* begin, Position* end)
{
    std::vector<std::string> parts;
    boost::split(parts, reg, boost::is_any_of(":"), boost::token_compress_on);

    if (parts.empty() || parts.size() > 2) {
        throw RegionStringException{reg};
    }

    // given name only, default min,max intervals
    if (parts.size() == 1) {
        *begin = 0;
        *end = 1 << 29;
    }

    // parse interval from input
    else if (parts.size() == 2) {
        std::vector<std::string> intervalParts;
        boost::split(intervalParts, parts.at(1), boost::is_any_of("-"), boost::token_compress_on);
        if (intervalParts.empty() || intervalParts.size() > 2) {
            throw RegionStringException{reg};
        }
        *begin = std::stoi(intervalParts.at(0));
        *end = std::stoi(intervalParts.at(1));
    }

    return parts.at(0);
}

}  // namespace

GenomicInterval::GenomicInterval(std::string name, Position start, Position stop)
    : name_{std::move(name)}, interval_{std::move(start), std::move(stop)}
{}

GenomicInterval::GenomicInterval(const std::string& samtoolsRegionString)
{
    Position begin = UNMAPPED_POSITION;
    Position end = UNMAPPED_POSITION;

    name_ = parseRegionString(samtoolsRegionString, &begin, &end);
    if ((begin == UNMAPPED_POSITION) || (end == UNMAPPED_POSITION)) {
        throw RegionStringException{samtoolsRegionString};
    }
    interval_ = Data::Interval(begin, end);
}

bool GenomicInterval::operator==(const GenomicInterval& other) const noexcept
{
    return std::tie(name_, interval_) == std::tie(other.name_, other.interval_);
}

bool GenomicInterval::operator!=(const GenomicInterval& other) const noexcept
{
    return !(*this == other);
}

bool GenomicInterval::ContainedBy(const GenomicInterval& other) const
{
    if (name_ != other.name_) {
        return false;
    }
    return interval_.ContainedBy(other.interval_);
}

bool GenomicInterval::Contains(const GenomicInterval& other) const
{
    if (name_ != other.name_) {
        return false;
    }
    return interval_.Contains(other.interval_);
}

bool GenomicInterval::Intersects(const GenomicInterval& other) const
{
    if (name_ != other.name_) {
        return false;
    }
    return interval_.Intersects(other.interval_);
}

Data::Interval GenomicInterval::Interval() const { return interval_; }

GenomicInterval& GenomicInterval::Interval(Data::Interval interval)
{
    interval_ = std::move(interval);
    return *this;
}

bool GenomicInterval::IsValid() const
{
    return (!name_.empty() && (interval_.Start() >= 0) && (interval_.End() >= 0) &&
            interval_.IsValid());
}

std::size_t GenomicInterval::Length() const { return interval_.Length(); }

std::string GenomicInterval::Name() const { return name_; }

GenomicInterval& GenomicInterval::Name(std::string name)
{
    name_ = std::move(name);
    return *this;
}

Position GenomicInterval::Start() const { return interval_.Start(); }

GenomicInterval& GenomicInterval::Start(const Position start)
{
    interval_.Start(start);
    return *this;
}

Position GenomicInterval::Stop() const { return interval_.End(); }

GenomicInterval& GenomicInterval::Stop(const Position stop)
{
    interval_.End(stop);
    return *this;
}

}  // namespace Data
}  // namespace PacBio

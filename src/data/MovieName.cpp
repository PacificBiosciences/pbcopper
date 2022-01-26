#include <pbcopper/data/MovieName.h>

#include <cstddef>

namespace PacBio {
namespace Data {

MovieName::MovieName(const std::string& instrumentName, const std::string& runStartTime)
    : partsCache_(nullptr)
{
    // construct name from parts
    std::string result;
    result.reserve(128);
    result += "m";
    result += instrumentName;
    result += "_";
    result += runStartTime;
    movieName_ = result;
    // don't update cache until actually requested
}

void MovieName::UpdatePartsCache() const
{
    // sanity checks
    assert(partsCache_ == nullptr);
    if (movieName_.empty()) {
        return;
    }

    // calculate name parts
    const char underscore = '_';
    const size_t firstUnderscore = movieName_.find(underscore);

    const char* movieCStr = movieName_.c_str();
    const char* nameStart = movieCStr + 1;
    const char* rstStart = movieCStr + firstUnderscore + 1;  // skip '_'
    const size_t nameSize = (firstUnderscore - 1);
    const size_t rstSize = (movieName_.size() - firstUnderscore) - 1;  // skip '\0 '

    // cache name parts
    partsCache_.reset(new PartsCache{
        std::string_view{nameStart, nameSize},  // instrumentName
        std::string_view{rstStart, rstSize}     // runStartTime
    });

    // checks - here? or elsewhere?
    if (partsCache_->instrumentName_.empty()) {
        throw std::runtime_error("[pbcopper] movie name ERROR: instrument name must not be empty");
    }
    if (partsCache_->runStartTime_.empty()) {
        throw std::runtime_error("[pbcopper] movie name ERROR: run start time must not be empty");
    }
}

}  // namespace Data
}  // namespace PacBio

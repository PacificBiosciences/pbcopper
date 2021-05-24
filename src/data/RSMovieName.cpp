// Author: Derek Barnett

#include <pbcopper/data/RSMovieName.h>

#include <cstddef>

namespace PacBio {
namespace Data {

RSMovieName::RSMovieName(const std::string& runStartTime, const std::string& instrumentSerialNumber,
                         const std::string& smrtCellBarcode, const std::string& setNumber,
                         const std::string& partNumber)
    : partsCache_(nullptr)
{
    // construct name from parts
    std::string result;
    result.reserve(128);
    result += "m";
    result += runStartTime;
    result += "_";
    result += instrumentSerialNumber;
    result += "_";
    result += smrtCellBarcode;
    result += "_";
    result += setNumber;
    result += "_";
    result += partNumber;
    movieName_ = result;

    // don't update cache until actually requested
}

void RSMovieName::UpdatePartsCache() const
{
    // sanity checks
    assert(partsCache_ == nullptr);
    if (movieName_.empty()) {
        return;
    }

    // calculate name parts
    const char underscore = '_';
    const size_t firstUnderscore = movieName_.find(underscore);
    const size_t secondUnderscore = movieName_.find(underscore, firstUnderscore + 1);
    const size_t thirdUnderscore = movieName_.find(underscore, secondUnderscore + 1);
    const size_t fourthUnderscore = movieName_.find(underscore, thirdUnderscore + 1);
    const size_t fifthUnderscore = movieName_.find(underscore, fourthUnderscore + 1);

    const char* movieCStr = movieName_.c_str();
    const char* rstStart = movieCStr + 1;  // skip 'm' and include first '_'
    const char* serialNumStart = movieCStr + secondUnderscore + 1;  // skip '_'
    const char* scbStart = movieCStr + thirdUnderscore + 1;         // skip '_'
    const char* setNumStart = movieCStr + fourthUnderscore + 1;     // skip '_'
    const char* partNumStart = movieCStr + fifthUnderscore + 1;     // skip '_'
    const size_t rstSize = (secondUnderscore - 1);
    const size_t serialNumSize = (thirdUnderscore - secondUnderscore) - 1;  // skip '_'
    const size_t scbSize = (fourthUnderscore - thirdUnderscore) - 1;        // skip '_'
    const size_t setNumSize = (fifthUnderscore - fourthUnderscore) - 1;     // skip '_'
    const size_t partNumSize = (movieName_.size() - fifthUnderscore) - 1;   // skip '\0 '

    // cache name parts
    partsCache_.reset(new RSMovieName::PartsCache{
        boost::string_ref{rstStart, rstSize},              // runStartTime
        boost::string_ref{serialNumStart, serialNumSize},  // serialNumber
        boost::string_ref{scbStart, scbSize},              // smrtCellBarcode
        boost::string_ref{setNumStart, setNumSize},        // setNumber
        boost::string_ref{partNumStart, partNumSize},      // partNumber
    });

    // checks - here? or elsewhere?
    if (partsCache_->runStartTime_.empty()) {
        throw std::runtime_error(
            "[pbcopper] RS movie name ERROR: run start time must not be empty");
    }
    if (partsCache_->serialNumber_.empty()) {
        throw std::runtime_error(
            "[pbcopper] RS movie name ERROR: instrument serial number must not be empty");
    }
    if (partsCache_->smrtCellBarcode_.empty()) {
        throw std::runtime_error(
            "[pbcopper] RS movie name ERROR: SMRT cell barcode must not be empty");
    }
    if (partsCache_->setNumber_.empty()) {
        throw std::runtime_error("[pbcopper] RS movie name ERROR: set number must not be empty");
    }
    if (partsCache_->partNumber_.empty()) {
        throw std::runtime_error("[pbcopper] RS movie name ERROR: part number must not be empty");
    }
}

}  // namespace Data
}  // namespace PacBio

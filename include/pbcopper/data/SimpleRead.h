// Author: Derek Barnett

#ifndef PBCOPPER_DATA_SIMPLEREAD_H
#define PBCOPPER_DATA_SIMPLEREAD_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

#include <boost/optional.hpp>

#include <pbcopper/data/Frames.h>
#include <pbcopper/data/Position.h>
#include <pbcopper/data/QualityValues.h>
#include <pbcopper/data/SNR.h>

namespace PacBio {
namespace Data {

class SimpleRead
{
public:
    SimpleRead(std::string name, std::string seq, QualityValues qualities, SNR snr);
    SimpleRead(std::string name, std::string seq, QualityValues qualities, SNR snr, Position qStart,
               Position qEnd);
    SimpleRead(std::string name, std::string seq, QualityValues qualities, SNR snr, Position qStart,
               Position qEnd, Frames pulseWidths, Frames ipd);

    // general data
    std::string Name;
    std::string Sequence;
    QualityValues Qualities;
    SNR SignalToNoise;
    Position QueryStart;
    Position QueryEnd;
    boost::optional<Frames> PulseWidths;
    boost::optional<Frames> IPD;
};

///
/// \brief
///
/// \param read
/// \param start
/// \param end
///
void ClipToQuery(SimpleRead& read, Position start, Position end);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_SIMPLEREAD_H

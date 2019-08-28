// Author: Armin Töpfer

#ifndef PBCOPPER_DATA_READ_H
#define PBCOPPER_DATA_READ_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/data/Accuracy.h>
#include <pbcopper/data/Frames.h>
#include <pbcopper/data/LocalContextFlags.h>
#include <pbcopper/data/Position.h>
#include <pbcopper/data/QualityValues.h>
#include <pbcopper/data/ReadId.h>
#include <pbcopper/data/SNR.h>

namespace PacBio {
namespace Data {

/// A Read contains the name, sequence, covariates, SNR, and associated model.
struct Read
{
    Read(Data::ReadId id, std::string seq, Frames pw, LocalContextFlags flags,
         Accuracy readAccuracy, SNR snr, std::string model);
    Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr);
    Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr,
         Position qStart, Position qEnd);
    Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr,
         Position qStart, Position qEnd, Frames pulseWidths, Frames ipd);

    ReadId Id;

    // base-level vectors
    std::string Seq;
    Frames PulseWidth;
    QualityValues Qualities;
    boost::optional<Frames> IPD;

    Position QueryStart = UnmappedPosition;
    Position QueryEnd = UnmappedPosition;

    LocalContextFlags Flags;
    Accuracy ReadAccuracy = 0;
    SNR SignalToNoise;
    std::string Model;
    bool FullLength;

    size_t Length() const;
    std::string FullName() const;

    Read ClipTo(const int32_t begin, const int32_t end) const;
};

void ClipToQuery(Read& read, Position start, Position end);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_READ_H

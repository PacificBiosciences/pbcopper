#ifndef PBCOPPER_DATA_READ_H
#define PBCOPPER_DATA_READ_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Accuracy.h>
#include <pbcopper/data/Frames.h>
#include <pbcopper/data/LocalContextFlags.h>
#include <pbcopper/data/Position.h>
#include <pbcopper/data/QualityValues.h>
#include <pbcopper/data/ReadId.h>
#include <pbcopper/data/SNR.h>

#include <iosfwd>
#include <string>
#include <tuple>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Data {

/// A Read contains the name, sequence, covariates, SNR, and associated model.
struct Read
{
    Read(Data::ReadId id, std::string seq, std::optional<Frames> pw, std::optional<Frames> ipd,
         LocalContextFlags flags, Accuracy readAccuracy, SNR snr, std::string model);
    Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr);
    Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr,
         Position qStart, Position qEnd);
    Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr,
         Position qStart, Position qEnd, Frames pulseWidths, Frames ipd);

    ReadId Id;

    // base-level vectors
    std::string Seq;
    std::optional<Frames> PulseWidth;
    QualityValues Qualities;
    std::optional<Frames> IPD;

    Position QueryStart = UNMAPPED_POSITION;
    Position QueryEnd = UNMAPPED_POSITION;

    LocalContextFlags Flags = LocalContextFlags::NO_LOCAL_CONTEXT;
    Accuracy ReadAccuracy = 0;
    SNR SignalToNoise;
    std::string Model;
    bool FullLength = false;

    std::int32_t Length() const;
    std::string FullName() const;

    ///
    /// Get a clipped copy of this read.
    ///
    /// \note This clipping is done on * internal * coordinates, rather than
    ///       query/reference coordinates.
    ///
    /// \note No bounds checking is performed.
    ///
    Read ClipTo(std::int32_t begin, std::int32_t end) const;
};

std::ostream& operator<<(std::ostream& os, const Read& read);

void ClipToQuery(Read& read, Position start, Position end);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_READ_H

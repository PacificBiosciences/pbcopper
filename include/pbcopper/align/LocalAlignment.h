#ifndef PBCOPPER_ALIGN_LOCALALIGNMENT_H
#define PBCOPPER_ALIGN_LOCALALIGNMENT_H

//
// SIMD local (Smith-Waterman) alignment score
//

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

namespace PacBio {
namespace Align {

class LocalAlignment
{
public:
    LocalAlignment(std::int32_t targetBegin, std::int32_t targetEnd, std::int32_t queryBegin,
                   std::int32_t queryEnd, std::int32_t mismatches, std::uint16_t score,
                   std::vector<std::uint32_t> cigar, std::string cigarString);

    LocalAlignment(const LocalAlignment&) = delete;
    LocalAlignment(LocalAlignment&&) = default;
    LocalAlignment& operator=(const LocalAlignment&) = delete;
    LocalAlignment& operator=(LocalAlignment&&) = default;

public:
    std::int32_t TargetBegin() const { return targetBegin_; }
    std::int32_t TargetEnd() const { return targetEnd_; }
    std::int32_t QueryBegin() const { return queryBegin_; }
    std::int32_t QueryEnd() const { return queryEnd_; }
    std::int32_t NumMismatches() const { return mismatches_; }
    std::uint16_t Score() const { return score_; }
    std::vector<std::uint32_t> Cigar() const { return cigar_; }
    std::string CigarString() const { return cigarString_; }

private:
    std::int32_t targetBegin_;
    std::int32_t targetEnd_;
    std::int32_t queryBegin_;
    std::int32_t queryEnd_;
    std::int32_t mismatches_;
    std::uint16_t score_;
    std::vector<std::uint32_t> cigar_;
    std::string cigarString_;
};

struct LocalAlignConfig
{
public:
    std::uint8_t MatchScore;
    std::uint8_t MismatchPenalty;
    std::uint8_t GapOpenPenalty;
    std::uint8_t GapExtendPenalty;

public:
    static LocalAlignConfig Default();
};

///
/// \brief LocalAlign
///
/// \param target
/// \param query
/// \param config
///
/// \return
///
LocalAlignment LocalAlign(const std::string& target, const std::string& query,
                          const LocalAlignConfig& config = LocalAlignConfig::Default());

///
/// \brief LocalAlign
///
/// \param target
/// \param queries
/// \param config
///
/// \return
///
std::vector<LocalAlignment> LocalAlign(
    const std::string& target, const std::vector<std::string>& queries,
    const LocalAlignConfig& config = LocalAlignConfig::Default());

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_LOCALALIGNMENT_H

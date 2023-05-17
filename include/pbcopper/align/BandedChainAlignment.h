#ifndef PBCOPPER_ALIGN_BANDEDCHAINALIGNMENT_H
#define PBCOPPER_ALIGN_BANDEDCHAINALIGNMENT_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/Seed.h>

#include <string>
#include <vector>

#include <cstddef>

//
// Support for banded alignment of seed chains
//

namespace PacBio {
namespace Align {

/// \brief The BandedChainAlignConfig struct provides various parameters used
///        by the BandedChainAlign algorithm.
///
struct BandedChainAlignConfig
{
public:
    float matchScore_;
    float mismatchPenalty_;
    float gapOpenPenalty_;
    float gapExtendPenalty_;
    std::size_t bandExtend_;

public:
    static BandedChainAlignConfig Default();
};

///
/// \brief The BandedChainAlignment struct contains the result of a
///        call to BandedChainAlign.
///
class BandedChainAlignment
{
public:
    BandedChainAlignConfig config_;
    std::string target_;
    std::string query_;
    std::string alignedTarget_;
    std::string alignedQuery_;
    std::string transcript_;
    int64_t score_;

public:
    BandedChainAlignment() = default;
    BandedChainAlignment(const BandedChainAlignConfig& config, std::string target,
                         std::string query, const std::string& transcript);
    BandedChainAlignment(const BandedChainAlignConfig& config, const char* target,
                         std::size_t targetLen, const char* query, std::size_t queryLen,
                         const std::string& transcript);

public:
    float Identity() const;
    int64_t Score() const;
};

///
/// \brief BandedChainAlign
///
///  Peforms banded alignment over a list of seeds.
///
/// \param target     target (reference) sequence
/// \param targetLen  target length
/// \param query      query sequence
/// \param queryLen   query length
/// \param seeds      pre-computed seeds to guide alignment
/// \param config     algorithm parameters
///
/// \return alignment results (pairwise alignment, score, etc)
///
BandedChainAlignment BandedChainAlign(
    const char* target, std::size_t targetLen, const char* query, std::size_t queryLen,
    const std::vector<PacBio::Align::Seed>& seeds,
    const BandedChainAlignConfig& config = BandedChainAlignConfig::Default());

///
/// \brief BandedChainAlign
///
///  Peforms banded alignment over a list of seeds.
///
///  This is an overloaded method.
///
/// \param target     target (reference) sequence
/// \param query      query sequence
/// \param seeds      pre-computed seeds to guide alignment
/// \param config     algorithm parameters
///
/// \return alignment results (pairwise alignment, score, etc)
///
BandedChainAlignment BandedChainAlign(
    const std::string& target, const std::string& query,
    const std::vector<PacBio::Align::Seed>& seeds,
    const BandedChainAlignConfig& config = BandedChainAlignConfig::Default());

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_BANDEDCHAINALIGNMENT_H

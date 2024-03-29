#ifndef PBCOPPER_ALIGN_BCALIGNBLOCKS_H
#define PBCOPPER_ALIGN_BCALIGNBLOCKS_H

//
// Alignment blocks for banded chain alignment
//

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/BandedChainAlignment.h>
#include <pbcopper/align/Seed.h>

#include <string>
#include <utility>
#include <vector>

#include <cfloat>

namespace PacBio {
namespace Align {
namespace Internal {

///
/// \brief The AlignInput struct acts as a "parameter-pack" for simplifiying
///        method signatures.
///
struct AlignInput
{
    const char* target;
    std::size_t tLen;
    const char* query;
    std::size_t qLen;
};

///
/// \brief The BandedGlobalAlignBlock class provides a reusable alignment
///        matrix for performing a banded, global alignment.
///
/// \note Currently only intended for use within the BandeChainAlign algorithm.
///
class BandedGlobalAlignBlock
{
public:
    BandedGlobalAlignBlock(const BandedChainAlignConfig& config) : config_(config)  // icc 17 hack
    {}

public:
    ///
    /// \brief Align
    ///
    /// Aligns query to target, using a banded-global alignment, with affine
    /// gap-penalties
    ///
    /// \param target   target sequence
    /// \param query    query sequence
    /// \param seed     hit region
    /// \return
    ///
    std::string Align(const char* target, const char* query, PacBio::Align::Seed seed);

private:
    std::pair<std::size_t, std::size_t> BacktraceStart(std::size_t tLen, std::size_t qLen) const;

    std::size_t IndexFor(std::size_t i, std::size_t j) const;

    void Init(std::size_t tLen, std::size_t qLen);
    std::size_t InitLookup(std::size_t tLen, std::size_t qLen);
    void InitScores(std::size_t tLen, std::size_t qLen, std::size_t n);

private:
    BandedChainAlignConfig config_;

    ///
    /// \brief The LookupElement struct helps provide mappings from the 2-D
    ///        (conceptual) matrix coordinates to the linear storage vectors.
    ///
    /// \sa IndexFor
    ///
    struct LookupElement
    {
        std::size_t arrayStart_;
        std::size_t jBegin_;
        std::size_t jEnd_;

        LookupElement(const std::size_t start, const std::size_t jBeg, const std::size_t jEnd)
            : arrayStart_{start}, jBegin_{jBeg}, jEnd_{jEnd}
        {}
    };

    std::vector<LookupElement> lookup_;

    std::vector<float> matchScores_;
    std::vector<float> gapScores_;
};

///
/// \brief The StandardGlobalAlignBlock class probides a reusable alignment
///        matrix for standard (non-banded) global alignment.
///
/// \note Currently only intended for use within the BandeChainAlign algorithm.
///
class StandardGlobalAlignBlock
{
public:
    StandardGlobalAlignBlock(const BandedChainAlignConfig& config) : config_(config)  // icc 17 hack
    {}

public:
    ///
    /// \brief Align
    ///
    /// Aligns query to target, using a global (NW) alignment, with affine
    /// gap-penalties
    ///
    /// \param target
    /// \param tLen
    /// \param query
    /// \param qLen
    /// \return
    ///
    std::string Align(const char* target, std::size_t tLem, const char* query, std::size_t qLen);

private:
    std::pair<std::size_t, std::size_t> BacktraceStart(std::size_t tLen, std::size_t qLen) const;

    void Init(std::size_t tLen, std::size_t qLen);

private:
    BandedChainAlignConfig config_;

    using Matrix_t = std::vector<std::vector<float>>;
    Matrix_t matchScores_;
    Matrix_t gapScores_;
};

}  // namespace Internal
}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_BCALIGNBLOCKS_H

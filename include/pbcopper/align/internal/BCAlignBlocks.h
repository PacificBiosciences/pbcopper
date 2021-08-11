#ifndef PBCOPPER_ALIGN_BCALIGNBLOCKS_H
#define PBCOPPER_ALIGN_BCALIGNBLOCKS_H

//
// Alignment blocks for banded chain alignment
//

#include <cfloat>

#include <string>
#include <utility>
#include <vector>

#include <pbcopper/align/BandedChainAlignment.h>
#include <pbcopper/align/Seed.h>

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
    size_t tLen;
    const char* query;
    size_t qLen;
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
    {
    }

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
    std::pair<size_t, size_t> BacktraceStart(const size_t tLen, const size_t qLen) const;

    size_t IndexFor(const size_t i, const size_t j) const;

    void Init(const size_t tLen, const size_t qLen);
    size_t InitLookup(const size_t tLen, const size_t qLen);
    void InitScores(const size_t tLen, const size_t qLen, const size_t n);

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
        size_t arrayStart_;
        size_t jBegin_;
        size_t jEnd_;

        LookupElement(const size_t start, const size_t jBeg, const size_t jEnd)
            : arrayStart_{start}, jBegin_{jBeg}, jEnd_{jEnd}
        {
        }
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
    {
    }

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
    std::string Align(const char* target, const size_t tLem, const char* query, const size_t qLen);

private:
    std::pair<size_t, size_t> BacktraceStart(const size_t tLen, const size_t qLen) const;

    void Init(const size_t tLen, const size_t qLen);

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

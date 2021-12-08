#ifndef PBCOPPER_ALIGN_PAIRWISEALIGNMENT_H
#define PBCOPPER_ALIGN_PAIRWISEALIGNMENT_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/AlignConfig.h>

#include <string>
#include <vector>

namespace PacBio {
namespace Align {

enum class LRType
{
    LEFT,
    RIGHT
};

/// \brief A pairwise alignment
class PairwiseAlignment
{
private:
    std::string target_;
    std::string query_;
    std::string transcript_;
    size_t refStart_;
    size_t refEnd_;

public:
    // either left- or right- justify indels
    void Justify(LRType);

public:
    // target string, including gaps; usually the "reference"
    std::string Target() const;

    // query string, including gaps; usually the "read"
    std::string Query() const;

    // transcript as defined by Gusfield pg 215.
    std::string Transcript() const;

    // starting position in the reference sequence
    size_t ReferenceStart() const;

    // ending position in the reference sequence
    size_t ReferenceEnd() const;

    // vector of target positions for each position in alignment
    std::vector<int> TargetPositions() const;

public:
    float Accuracy() const;
    int Matches() const;
    int Errors() const;
    int Mismatches() const;
    int Insertions() const;
    int Deletions() const;
    int Length() const;

public:
    PairwiseAlignment(std::string target, std::string query, size_t refStart = 0,
                      size_t refEnd = 0);

    static PairwiseAlignment* FromTranscript(const std::string& transcript,
                                             const std::string& unalnTarget,
                                             const std::string& unalnQuery);

    PairwiseAlignment ClippedTo(size_t refStart, size_t refEnd);
};

PairwiseAlignment* Align(const std::string& target, const std::string& query, int* score,
                         AlignConfig config = AlignConfig::Default());

PairwiseAlignment* Align(const std::string& target, const std::string& query,
                         AlignConfig config = AlignConfig::Default());

// These calls return an array, same len as target, containing indices into the query string.
std::vector<int> TargetToQueryPositions(const std::string& transcript);
std::vector<int> TargetToQueryPositions(const PairwiseAlignment& aln);

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_PAIRWISEALIGNMENT_H

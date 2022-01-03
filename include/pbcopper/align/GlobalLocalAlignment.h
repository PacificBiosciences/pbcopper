#ifndef PBCOPPER_ALIGN_GLOBALLOCALALIGNMENT_H
#define PBCOPPER_ALIGN_GLOBALLOCALALIGNMENT_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace PacBio {
namespace Align {

struct GlobalLocalParameters
{
    int32_t MatchScore{};
    int32_t MismatchPenalty{};
    int32_t DeletionPenalty{};
    int32_t InsertionPenalty{};
    int32_t BranchPenalty{};
};

struct GlobalLocalResult
{
    int32_t MaxScore = -1;
    int32_t EndPos = -1;
};

/// Fills out a supplied dynamic programming matrix.
void GlobalLocalComputeMatrix(const char* query, int32_t queryLength, const char* target,
                              int32_t targetLength, bool globalInQuery,
                              const GlobalLocalParameters& parameters,
                              std::vector<int32_t>& matrix) noexcept;

/// \brief Align query against target given the parameters.
///        Global in the query, but missing bases from the query at the start of the target are not penalized.
///        Local in the target.
///        Return a max score in the last row and its target position.
///        Attention: This is the slowest of all available APIs,
///                   as memory has to allocated for every call.
GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& target,
                                   const GlobalLocalParameters& parameters) noexcept;

/// \brief Align query against target given the parameters.
///        Global in the query, but missing bases from the query at the start of the target are not penalized.
///        Local in the target.
///        Return a max score in the last row and its target position.
GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& target,
                                   const GlobalLocalParameters& parameters,
                                   std::vector<int32_t>& matrix) noexcept;

/// \brief Traverse the last row of a DP matrix (i.e. representing
///        alignments terminating with the last base of the query
///        sequence) and return the max score and it's position
GlobalLocalResult GlobalLocalLastRowMax(const std::vector<int32_t>& matrix, int32_t queryLength,
                                        int32_t targetLength) noexcept;

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_GLOBALLOCALALIGNMENT_H

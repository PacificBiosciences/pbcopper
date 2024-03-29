#ifndef PBCOPPER_ALIGN_GLOBALLOCALALIGNMENT_H
#define PBCOPPER_ALIGN_GLOBALLOCALALIGNMENT_H

#include <cstdint>
#include <string>
#include <vector>

namespace PacBio {
namespace Align {

// Scores and penalties
struct GlobalLocalParameters
{
    std::int32_t MatchScore{};
    std::int32_t MismatchPenalty{};
    std::int32_t DeletionPenalty{};
    std::int32_t InsertionPenalty{};
    std::int32_t BranchPenalty{};
    std::int32_t MergePenalty{};
};

struct GlobalLocalResult
{
    std::int32_t MaxScore{};
    std::int32_t EndPos{};
};

// This storage can be used to further improve runtime
struct GlobalLocalStorage
{
    std::vector<std::int32_t> Columns;
};

/// \brief Align a known query against read given the parameters.
///        Global in the query, but missing bases from the query at the start of the read are not penalized.
///        Local in the read.
///        Return a max score in the last row and its read position.
///        Attention: This is the slower API, requiring memory allocation.
GlobalLocalResult GlobalLocalAlign(const char* query, std::int32_t queryLength, const char* read,
                                   std::int32_t readLength,
                                   const GlobalLocalParameters& parameters) noexcept;

/// \brief Align a known query against read given the parameters.
///        Global in the query, but missing bases from the query at the start of the read are not penalized.
///        Local in the read.
///        Return a max score in the last row and its read position.
///        Attention: This is the slower API, requiring memory allocation.
GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& read,
                                   const GlobalLocalParameters& parameters) noexcept;

/// \brief Align a known query against read given the parameters.
///        Global in the query, but missing bases from the query at the start of the read are not penalized.
///        Local in the read.
///        Return a max score in the last row and its read position.
///        Attention: This is the faster API, as memory can be reused.
GlobalLocalResult GlobalLocalAlign(const char* query, std::int32_t queryLength, const char* read,
                                   std::int32_t readLength, const GlobalLocalParameters& parameters,
                                   GlobalLocalStorage& storage) noexcept;

/// \brief Align a known query against read given the parameters.
///        Global in the query, but missing bases from the query at the start of the read are not penalized.
///        Local in the read.
///        Return a max score in the last row and its read position.
///        Attention: This is the faster API, as memory can be reused.
GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& read,
                                   const GlobalLocalParameters& parameters,
                                   GlobalLocalStorage& storage) noexcept;

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_GLOBALLOCALALIGNMENT_H

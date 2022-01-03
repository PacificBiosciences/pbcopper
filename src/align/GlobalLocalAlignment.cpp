#include <pbcopper/align/GlobalLocalAlignment.h>

#include <pbcopper/utility/Ssize.h>

#include <algorithm>
#include <limits>

namespace PacBio {
namespace Align {

GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& target,
                                   const GlobalLocalParameters& parameters) noexcept
{
    std::vector<int32_t> matrix;
    return GlobalLocalAlign(query, target, parameters, matrix);
}

GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& target,
                                   const GlobalLocalParameters& parameters,
                                   std::vector<int32_t>& matrix) noexcept
{
    const int32_t queryLength = query.size();
    const int32_t targetLength = target.size();
    GlobalLocalComputeMatrix(query.c_str(), queryLength, target.c_str(), targetLength, false,
                             parameters, matrix);
    return GlobalLocalLastRowMax(matrix, queryLength, targetLength);
}

void GlobalLocalComputeMatrix(const char* const query, const int32_t queryLength,
                              const char* const target, const int32_t targetLength,
                              const bool globalInQuery, const GlobalLocalParameters& parameters,
                              std::vector<int32_t>& matrix) noexcept
{
    // The matrix has m rows for the query and
    //                n column for the target
    const int32_t m = queryLength + 1;
    const int32_t n = targetLength + 1;

    if (Utility::Ssize(matrix) < (m * n)) {
        matrix.resize(m * n);
    }

    matrix[0] = 0;

    // Global in query means that we penalize missing bases of the query, if the
    // target is truncated, only at the target start.
    // If we don't penalize it, we allow that the target might have been trimmed
    // in previous processing steps.
    //
    // Example
    // Target:    AGATATGCCAG
    // Query : GGTAGATAT
    if (globalInQuery) {
        for (int32_t i = 1; i < m; ++i) {
            matrix[i * n] = i * parameters.DeletionPenalty;
        }
    } else {
        for (int32_t i = 1; i < m; ++i) {
            matrix[i * n] = 0;
        }
    }

    for (int32_t j = 1; j < n; ++j) {
        matrix[j] = 0;
    }

    // The matrix has i for rows, query bases, and j for columns, target bases.
    // It's a 1D vector with a row layout. The first n cells are matching the
    // first query base with 1:m target bases.
    // Keep in mind that there the first column and row are not matching any
    // bases but are used for initialization.
    const int32_t insertionDelta = parameters.BranchPenalty - parameters.InsertionPenalty;
    for (int32_t i = 1; i < m; ++i) {
        const char iQuery = query[i];
        const char iBeforeQuery = query[i - 1];
        if (i < m - 1) {
            for (int32_t j = 1; j < n; ++j) {
                // We prefer more arithmetic operations than accessing memory
                // in branches.
                int32_t a{matrix[(i - 1) * n + j - 1]};
                int32_t b{matrix[i * n + j - 1] + parameters.BranchPenalty};
                const int32_t c{matrix[(i - 1) * n + j] + parameters.DeletionPenalty};
                if (target[j - 1] == iBeforeQuery) {
                    a += parameters.MatchScore;
                } else {
                    a += parameters.MismatchPenalty;
                }
                if (target[j - 1] != iQuery) {
                    b -= insertionDelta;
                }
                matrix[i * n + j] = std::max(a, std::max(b, c));
            }
        } else {
            for (int32_t j = 1; j < n; ++j) {
                int32_t a = matrix[(i - 1) * n + j - 1];
                const int32_t b{matrix[i * n + j - 1] + parameters.InsertionPenalty};
                const int32_t c{matrix[(i - 1) * n + j] + parameters.DeletionPenalty};
                if (target[j - 1] == iBeforeQuery) {
                    a += parameters.MatchScore;
                } else {
                    a += parameters.MismatchPenalty;
                }
                matrix[i * n + j] = std::max(a, std::max(b, c));
            }
        }
    }
}

GlobalLocalResult GlobalLocalLastRowMax(const std::vector<int32_t>& matrix,
                                        const int32_t queryLength,
                                        const int32_t targetLength) noexcept
{
    // Calculate the starting position of the last row
    const int32_t m = queryLength + 1;
    const int32_t n = targetLength + 1;
    const int32_t beginLastRow = (m - 1) * n;

    // Find maximal score in last row and it's position
    int32_t maxScore = std::numeric_limits<int32_t>::min();
    int32_t endPos = 0;
    for (int32_t j = 0; j < n; ++j) {
        if (matrix[beginLastRow + j] > maxScore) {
            maxScore = matrix[beginLastRow + j];
            endPos = j;
        }
    }

    // Return the maximum score and position
    return {maxScore, endPos};
}
}  // namespace Align
}  // namespace PacBio

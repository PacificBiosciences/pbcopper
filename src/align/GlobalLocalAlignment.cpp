#include <pbcopper/align/GlobalLocalAlignment.h>

#include <pbcopper/utility/Ssize.h>

#include <algorithm>

namespace PacBio {
namespace Align {
namespace {
/// \brief Traverse the last row of a DP matrix (i.e. representing
///        alignments terminating with the last base of the query
///        sequence) and return the max score and it's position
GlobalLocalResult GlobalLocalLastRowMax(const std::vector<int32_t>& lastRow) noexcept
{
    const auto maxElementIt = std::max_element(std::cbegin(lastRow), std::cend(lastRow));
    const int32_t maxElementPos = maxElementIt - std::cbegin(lastRow);
    return {*maxElementIt, maxElementPos};
}
}

GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& read,
                                   const GlobalLocalParameters& parameters) noexcept
{
    GlobalLocalStorage storage{};
    return GlobalLocalAlign(query.c_str(), query.size(), read.c_str(), read.size(), parameters,
                            storage);
}

GlobalLocalResult GlobalLocalAlign(const char* const query, const int32_t queryLength,
                                   const char* const read, const int32_t readLength,
                                   const GlobalLocalParameters& parameters) noexcept
{
    GlobalLocalStorage storage{};
    return GlobalLocalAlign(query, queryLength, read, readLength, parameters, storage);
}

GlobalLocalResult GlobalLocalAlign(const std::string& query, const std::string& read,
                                   const GlobalLocalParameters& parameters,
                                   GlobalLocalStorage& storage) noexcept
{
    return GlobalLocalAlign(query.c_str(), query.size(), read.c_str(), read.size(), parameters,
                            storage);
}

GlobalLocalResult GlobalLocalAlign(const char* const query, const int32_t queryLength,
                                   const char* const read, const int32_t readLength,
                                   const GlobalLocalParameters& parameters,
                                   GlobalLocalStorage& storage) noexcept
{
    // The lastRow has m rows for the query and
    //                 n column for the read
    const int32_t m = queryLength + 1;
    const int32_t n = readLength + 1;

    std::vector<int32_t> lastRow(readLength);

    // We only store two columns as we do not compute the traceback.
    // Both columns are stored in one contiguous memory block.
    // Offsets determine where columns start.
    // Columns are swapped after each outer loop of read bases.
    if (Utility::Ssize(storage.Columns) < m * 2) {
        storage.Columns.resize(m * 2, 0);
    }
    auto* const col = storage.Columns.data();
    int32_t prevOffset = 0;
    int32_t curOffset = m;

    // Initialize previous column, after swapping, to zero
    std::fill_n(col + curOffset, m, 0);

    // Deltas are used to reduce number of instructions in the inner loop
    const int32_t mismatchDelta = parameters.MatchScore - parameters.MismatchPenalty;
    const int32_t insertionDelta = parameters.BranchPenalty - parameters.InsertionPenalty;
    const int32_t deletionDelta = parameters.MergePenalty - parameters.DeletionPenalty;

    // The matrix has i for rows, query bases, and j for columns, read bases.
    // Outer loop is over read bases und inner loop over query bases.
    // Insertion and deletions are with respect to the read that is horizontally.
    for (int32_t j = 1; j < n; ++j) {  // rows
        std::swap(prevOffset, curOffset);
        const char prevRead = read[j - 1];
        const int32_t readMismatch = (prevRead != read[j]);
        for (int32_t i = 1; i < m; ++i) {  // cols

            // Match/mismatch
            const int32_t a{col[i - 1 + prevOffset] + parameters.MatchScore -
                            (mismatchDelta * (prevRead != query[i - 1]))};

            // Insertion in the read or branch if the current and last read sequence are identical
            const int32_t b{col[i + prevOffset] + parameters.BranchPenalty -
                            (insertionDelta * readMismatch)};

            // Deletion in the read or merge if the current and last read sequence are identical
            const int32_t c{col[i - 1 + curOffset] + parameters.MergePenalty -
                            (deletionDelta * (query[i - 1] != query[i]))};

            col[i + curOffset] = std::max(a, std::max(b, c));
        }
        lastRow[j - 1] = col[curOffset + m - 1];
    }

    return GlobalLocalLastRowMax(lastRow);
}

}  // namespace Align
}  // namespace PacBio

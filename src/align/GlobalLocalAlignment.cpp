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

    if (Utility::Ssize(storage.Col0) < m) {
        storage.Col0.resize(m, 0);
        storage.Col1.resize(m, 0);
    }
    decltype(storage.Col0)* colCur = &storage.Col0;
    decltype(storage.Col1)* colPrev = &storage.Col1;

    const int32_t mismatchDelta = parameters.MatchScore - parameters.MismatchPenalty;
    const int32_t insertionDelta = parameters.BranchPenalty - parameters.InsertionPenalty;
    const int32_t deletionDelta = parameters.MergePenalty - parameters.DeletionPenalty;

    // The matrix has i for rows, query bases, and j for columns, read bases.
    // Outer loop is over read bases und inner loop over query bases.
    //
    // We only store two columns as we do not compute the traceback.
    // Columns are swapped after each outer loop of read bases.
    //
    // Insertion and deletions are with respect to the read that is horizontally.
    for (int32_t j = 1; j < n; ++j) {  // rows
        std::swap(colCur, colPrev);
        const char prevRead = read[j - 1];
        const int32_t readMismatch = prevRead != read[j];
        for (int32_t i = 1; i < m; ++i) {  // cols

            // Match/mismatch
            const int32_t a{(*colPrev)[i - 1] + parameters.MatchScore -
                            (mismatchDelta * static_cast<int32_t>(prevRead != query[i - 1]))};

            // Insertion in the read or branch if the current and last read sequence are identical
            const int32_t b{(*colPrev)[i] + parameters.BranchPenalty -
                            (insertionDelta * readMismatch)};

            // Deletion in the read or merge if the current and last read sequence are identical
            const int32_t c{(*colCur)[i - 1] + parameters.MergePenalty -
                            (deletionDelta * (query[i - 1] != query[i]))};

            (*colCur)[i] = std::max(a, std::max(b, c));
        }
        lastRow[j - 1] = (*colCur).back();
    }

    return GlobalLocalLastRowMax(lastRow);
}

}  // namespace Align
}  // namespace PacBio

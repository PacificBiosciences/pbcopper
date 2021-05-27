#include <pbcopper/align/PairwiseAlignment.h>

#include <cassert>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <boost/assert.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <pbcopper/utility/MinMax.h>
#include <pbcopper/utility/SequenceUtils.h>

namespace PacBio {
namespace Align {
namespace {

constexpr int ArgMax3(int a, int b, int c)
{
    if (a >= b && a >= c) {
        return 0;
    } else if (b >= c) {
        return 1;
    } else {
        return 2;
    }
}

}  // namespace

namespace internal {

bool Rewrite2L(std::string* target, std::string* query, std::string* transcript, const size_t i)
{
    char& t0 = (*target)[i];
    char& t1 = (*target)[i + 1];
    char& q0 = (*query)[i];
    char& q1 = (*query)[i + 1];
    char& x0 = (*transcript)[i];
    char& x1 = (*transcript)[i + 1];

    // t: XX    XX
    //    |* -> *|
    // q: X-    -X
    if (q1 == '-' && t0 == t1 && t1 == q0) {
        std::swap(q0, q1);
        std::swap(x0, x1);
        return true;
    }
    // t: X-    -X
    //    |* -> *|
    // q: XX    XX
    else if (t1 == '-' && q0 == q1 && q1 == t0) {
        std::swap(t0, t1);
        std::swap(x0, x1);
        return true;
    }
    return false;
}

bool Rewrite3L(std::string* target, std::string* query, std::string* transcript, const size_t i)
{
    char& t0 = (*target)[i];
    char& t2 = (*target)[i + 2];
    char& q0 = (*query)[i];
    char& q2 = (*query)[i + 2];
    char& x0 = (*transcript)[i];
    char& x2 = (*transcript)[i + 2];

    // t: X--    --X
    //    |** -> **|
    // q: XYX    XYX
    if (t0 == q2 && transcript->substr(i, 3) == "MII") {
        std::swap(t0, t2);
        std::swap(x0, x2);
        return true;
    }
    // t: XYX    XYX
    //    |** -> **|
    // q: X--    --X
    else if (q0 == t2 && transcript->substr(i, 3) == "MDD") {
        std::swap(q0, q2);
        std::swap(x0, x2);
        return true;
    }
    return false;
}

bool Rewrite2R(std::string* target, std::string* query, std::string* transcript, const size_t i)
{
    char& t0 = (*target)[i];
    char& t1 = (*target)[i + 1];
    char& q0 = (*query)[i];
    char& q1 = (*query)[i + 1];
    char& x0 = (*transcript)[i];
    char& x1 = (*transcript)[i + 1];

    // t: XX    XX
    //    *| -> |*
    // q: -X    X-
    if (q0 == '-' && t0 == t1 && t1 == q1) {
        std::swap(q0, q1);
        std::swap(x0, x1);
        return true;
    }
    // t: -X    X-
    //    *| -> |*
    // q: XX    XX
    else if (t0 == '-' && q0 == q1 && q1 == t1) {
        std::swap(t0, t1);
        std::swap(x0, x1);
        return true;
    }
    return false;
}

bool Rewrite3R(std::string* target, std::string* query, std::string* transcript, const size_t i)
{
    char& t0 = (*target)[i];
    char& t2 = (*target)[i + 2];
    char& q0 = (*query)[i];
    char& q2 = (*query)[i + 2];
    char& x0 = (*transcript)[i];
    char& x2 = (*transcript)[i + 2];

    // t: --X    X--
    //    **| -> |**
    // q: XYX    XYX
    if (q0 == t2 && transcript->substr(i, 3) == "IIM") {
        std::swap(t0, t2);
        std::swap(x0, x2);
        return true;
    }
    // t: XYX    XYX
    //    **| -> |**
    // q: --X    X--
    else if (t0 == q2 && transcript->substr(i, 3) == "DDM") {
        std::swap(q0, q2);
        std::swap(x0, x2);
        return true;
    }
    return false;
}

}  // namespace internal

std::string PairwiseAlignment::Target() const { return target_; }

std::string PairwiseAlignment::Query() const { return query_; }

size_t PairwiseAlignment::ReferenceStart() const { return refStart_; }

size_t PairwiseAlignment::ReferenceEnd() const { return refEnd_; }

float PairwiseAlignment::Accuracy() const { return (static_cast<float>(Matches())) / Length(); }

std::string PairwiseAlignment::Transcript() const { return transcript_; }

int PairwiseAlignment::Matches() const
{
    return std::count(transcript_.begin(), transcript_.end(), 'M');
}

int PairwiseAlignment::Errors() const { return Length() - Matches(); }

int PairwiseAlignment::Mismatches() const
{
    return std::count(transcript_.begin(), transcript_.end(), 'R');
}

int PairwiseAlignment::Insertions() const
{
    return std::count(transcript_.begin(), transcript_.end(), 'I');
}

int PairwiseAlignment::Deletions() const
{
    return std::count(transcript_.begin(), transcript_.end(), 'D');
}

void PairwiseAlignment::Justify(const LRType lr)
{
    const size_t L = Length();

    if (L < 2) {
        return;
    }

    while (true) {
        bool goAgain = false;
        if (lr == LRType::LEFT) {
            goAgain |= internal::Rewrite2L(&target_, &query_, &transcript_, L - 2);
            for (size_t i = L - 2; i > 0; --i) {
                goAgain |= internal::Rewrite2L(&target_, &query_, &transcript_, i - 1);
                goAgain |= internal::Rewrite3L(&target_, &query_, &transcript_, i - 1);
            }
        } else {
            for (size_t i = 0; i < L - 2; ++i) {
                goAgain |= internal::Rewrite2R(&target_, &query_, &transcript_, i);
                goAgain |= internal::Rewrite3R(&target_, &query_, &transcript_, i);
            }
            goAgain |= internal::Rewrite2R(&target_, &query_, &transcript_, L - 2);
        }
        if (!goAgain) {
            break;
        }
    }
}

int PairwiseAlignment::Length() const { return target_.length(); }

PairwiseAlignment::PairwiseAlignment(std::string target, std::string query, const size_t refStart,
                                     const size_t refEnd)
    : target_(std::move(target))
    , query_(std::move(query))
    , transcript_(target_.length(), 'Z')
    , refStart_(refStart)
    , refEnd_(refEnd)
{
    if (target_.length() != query_.length()) {
        throw std::invalid_argument(
            "[pbcopper] pairwise alignment ERROR: target length must equal query length");
    }
    for (unsigned int i = 0; i < target_.length(); i++) {
        char t = target_[i];
        char q = query_[i];
        char tr;

        if (t == '-' && q == '-') {
            throw std::invalid_argument{
                "[pbcopper] pairwise alignment ERROR: invalid target and query transcript"};
        } else if (t == q) {
            tr = 'M';
        } else if (t == '-') {
            tr = 'I';
        } else if (q == '-') {
            tr = 'D';
        } else {
            tr = 'R';
        }  // NOLINT

        transcript_[i] = tr;
    }
}

std::vector<int> PairwiseAlignment::TargetPositions() const
{
    std::vector<int> pos;
    pos.reserve(transcript_.size());

    int refPos = refStart_;
    for (const char c : transcript_) {
        if (c == 'M' || c == 'R' || c == 'D') {
            pos.push_back(refPos);
            refPos++;
        } else if (c == 'I') {
            pos.push_back(refPos);
        } else {
            throw std::runtime_error{
                "[pbcopper] pairwise alignment ERROR: unknown transcript code: " +
                std::string(1, c)};
        }
    }

    assert(pos.size() == transcript_.size());
    return pos;
}

PairwiseAlignment PairwiseAlignment::ClippedTo(const size_t refStart, const size_t refEnd)
{
    if (refStart >= refEnd || refStart >= ReferenceEnd() || refEnd <= ReferenceStart()) {
        throw std::runtime_error{
            "[pbcopper] pairwise alignment ERROR: requested clip range does not overlap alignment"};
    }

    const size_t clipRefStart = std::max(refStart, ReferenceStart());
    const size_t clipRefEnd = std::min(refEnd, ReferenceEnd());

    const std::vector<int> pos = TargetPositions();
    size_t clipStart =
        std::distance(pos.begin(), std::upper_bound(pos.begin(), pos.end(), clipRefStart));
    clipStart = clipStart > 0 ? clipStart - 1 : 0;
    const size_t clipEnd =
        std::distance(pos.begin(), std::lower_bound(pos.begin(), pos.end(), clipRefEnd));
    const size_t clipLength = clipEnd - clipStart;

    const std::string& clippedQuery = Query().substr(clipStart, clipLength);
    const std::string& clippedTarget = Target().substr(clipStart, clipLength);

    return PairwiseAlignment(clippedTarget, clippedQuery, clipRefStart, clipRefEnd);
}

PairwiseAlignment* Align(const std::string& target, const std::string& query, int* score,
                         AlignConfig config)
{
    using boost::numeric::ublas::matrix;

    const AlignParams& params = config.Params;
    if (config.Mode != AlignMode::GLOBAL && config.Mode != AlignMode::SEMIGLOBAL) {
        throw std::invalid_argument{
            "[pbcopper] pairwise alignment ERROR: only GLOBAL and SEMIGLOBAL alignments supported "
            "at present"};
    }

#ifndef NDEBUG
    // we can only align IUPAC bases, not gaps or any other characters
    const static boost::container::flat_set<char> validBases{
        'A', 'B', 'C', 'D', 'G', 'H', 'K', 'M', 'N', 'R', 'S', 'T', 'V', 'W', 'Y',
        'a', 'b', 'c', 'd', 'g', 'h', 'k', 'm', 'n', 'r', 's', 't', 'v', 'w', 'y'};

    for (char c : target) {
        BOOST_ASSERT_MSG(validBases.find(c) != validBases.cend(),
                         "target contains non-IUPAC bases");
    }

    for (char c : query) {
        BOOST_ASSERT_MSG(validBases.find(c) != validBases.cend(), "query contains non-IUPAC bases");
    }
#endif

    int I = query.length();
    int J = target.length();
    matrix<int> Score(I + 1, J + 1);

    Score(0, 0) = 0;
    for (int i = 1; i <= I; i++) {
        Score(i, 0) = i * params.Insert;
    }
    if (config.Mode == AlignMode::GLOBAL) {
        for (int j = 1; j <= J; j++) {
            Score(0, j) = j * params.Delete;
        }
    } else {
        for (int j = 1; j <= J; j++) {
            Score(0, j) = 0;
        }
    }
    for (int i = 1; i <= I; i++) {
        for (int j = 1; j <= J; j++) {
            bool isMatch = (query[i - 1] == target[j - 1]);
            Score(i, j) =
                Utility::Max(Score(i - 1, j - 1) + (isMatch ? params.Match : params.Mismatch),
                             Score(i - 1, j) + params.Insert, Score(i, j - 1) + params.Delete);
        }
    }
    if (score != nullptr) {
        *score = Score(I, J);
    }

    // Find the alignment end coordinate in the reference
    //  This is J if Global and the maximum scoring position if not
    int maxJ = J;
    if (config.Mode == AlignMode::SEMIGLOBAL) {
        int maxScore = std::numeric_limits<int>::min();
        for (int j = 1; j <= J; ++j) {
            if (Score(I, j) >= maxScore) {
                maxScore = Score(I, j);
                maxJ = j;
            }
        }
    }

    // Traceback, build up reversed aligned query, aligned target
    int i = I;
    int j = maxJ;
    std::string raQuery;
    std::string raTarget;
    while (i > 0 || (config.Mode == AlignMode::GLOBAL && j > 0)) {
        int move;
        if (i == 0) {
            move = 2;  // only deletion is possible
        } else if (j == 0) {
            move = 1;  // only insertion is possible
        } else {
            bool isMatch = (query[i - 1] == target[j - 1]);
            move = ArgMax3(Score(i - 1, j - 1) + (isMatch ? params.Match : params.Mismatch),
                           Score(i - 1, j) + params.Insert, Score(i, j - 1) + params.Delete);
        }
        // Incorporate:
        if (move == 0) {
            i--;
            j--;
            raQuery.push_back(query[i]);
            raTarget.push_back(target[j]);
        }
        // Insert:
        else if (move == 1) {
            i--;
            raQuery.push_back(query[i]);
            raTarget.push_back('-');
        }
        // Delete:
        else if (move == 2) {
            j--;
            raQuery.push_back('-');
            raTarget.push_back(target[j]);
        }
    }

    return new PairwiseAlignment(Utility::Reversed(raTarget), Utility::Reversed(raQuery),
                                 std::max(0, j - 1), maxJ - 1);
}

PairwiseAlignment* Align(const std::string& target, const std::string& query, AlignConfig config)
{
    return Align(target, query, nullptr, config);
}

//
//  Code for lifting target coordinates into query coordinates.
//

static bool addsToTarget(char transcriptChar)
{
    return (transcriptChar == 'M' || transcriptChar == 'R' || transcriptChar == 'D');
}

static int targetLength(const std::string& alignmentTranscript)
{
    return std::count_if(alignmentTranscript.begin(), alignmentTranscript.end(), addsToTarget);
}

#ifndef NDEBUG
static bool addsToQuery(char transcriptChar)
{
    return (transcriptChar == 'M' || transcriptChar == 'R' || transcriptChar == 'I');
}

static int queryLength(const std::string& alignmentTranscript)
{
    return std::count_if(alignmentTranscript.begin(), alignmentTranscript.end(), addsToQuery);
}
#endif  // !NDEBUG

// TargetPositionsInQuery:
// * Returns a vector of targetLength(transcript) + 1, which,
//   roughly speaking, indicates the positions in the query string of the
//   the characters in the target, as induced by an alignment with the
//   given transcript string.
// * More precisely, given an alignment (T, Q, X)  (x=transcript),
//   letting T[s, e) denote any slice of T,
//    - [s',e') denote the subslice of indices of Q aligned to T[s, e),
//    - ntp = NewTargetPositions(X)
//   we have
//      [s', e') = [ntp(s), ntp(e))
//
// * Ex:
//     MMM -> 0123
//     DMM -> 0012,  MMD -> 0122, MDM -> 0112
//     IMM -> 123,   MMI -> 013,  MIM -> 023
//     MRM, MIDM, MDIM -> 0123
std::vector<int> TargetToQueryPositions(const std::string& transcript)
{
    std::vector<int> ntp;
    ntp.reserve(targetLength(transcript) + 1);

    int queryPos = 0;
    for (const char c : transcript) {
        if (c == 'M' || c == 'R') {
            ntp.push_back(queryPos);
            queryPos++;
        } else if (c == 'D') {
            ntp.push_back(queryPos);
        } else if (c == 'I') {
            queryPos++;
        } else {
            throw std::runtime_error{
                "[pbcopper] pairwise alignment ERROR: unknown transcript code: " +
                std::string(1, c)};
        }
    }
    ntp.push_back(queryPos);

    assert(static_cast<int>(ntp.size()) == targetLength(transcript) + 1);
    assert(ntp[targetLength(transcript)] == queryLength(transcript));
    return ntp;
}

std::vector<int> TargetToQueryPositions(const PairwiseAlignment& aln)
{
    return TargetToQueryPositions(aln.Transcript());
}

// Build the alignment given the unaligned sequences and the transcript
// Returns NULL if transcript does not map unalnTarget into unalnQuery.
PairwiseAlignment* PairwiseAlignment::FromTranscript(const std::string& transcript,
                                                     const std::string& unalnTarget,
                                                     const std::string& unalnQuery)
{
    std::string alnTarget;
    std::string alnQuery;
    int tPos;
    int qPos;
    int tLen;
    int qLen;

    tLen = unalnTarget.length();
    qLen = unalnQuery.length();
    tPos = 0;
    qPos = 0;
    for (const char x : transcript) {
        if (tPos > tLen || qPos > qLen) {
            return nullptr;
        }

        char t = (tPos < tLen ? unalnTarget[tPos] : '\0');
        char q = (qPos < qLen ? unalnQuery[qPos] : '\0');

        switch (x) {
            case 'M':
                if (t != q) {
                    return nullptr;
                }
                alnTarget.push_back(t);
                alnQuery.push_back(q);
                tPos++;
                qPos++;
                break;
            case 'R':
                if (t == q) {
                    return nullptr;
                }
                alnTarget.push_back(t);
                alnQuery.push_back(q);
                tPos++;
                qPos++;
                break;
            case 'I':
                alnTarget.push_back('-');
                alnQuery.push_back(q);
                qPos++;
                break;
            case 'D':
                alnTarget.push_back(t);
                alnQuery.push_back('-');
                tPos++;
                break;
            default:
                return nullptr;
        }
    }
    // Didn't consume all of one of the strings
    if (tPos != tLen || qPos != qLen) {
        return nullptr;
    }

    // Provide another constructor to inject transcript?  Calculate transcript on the fly?
    return new PairwiseAlignment(alnTarget, alnQuery);
}

}  // namespace Align
}  // namespace PacBio

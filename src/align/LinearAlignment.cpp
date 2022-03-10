//
// This is the basic Hirschberg algo, not the affine variation by Myers and Miller.
// The tricky part of this code is that we use the a single array to store portions
// of two rows in the alignment matrices.
//
// Straightforward notes here: http://globin.cse.psu.edu/courses/fall2001/DP.pdf ;
// taken from "Recent Developments in Linear-Space Alignment Methods: A Survey".
// I follow them pretty closely except for the semiglobal alignment mode implemented
// here.
//
// For the affine (Gotoh) variation, not yet implemented, see Myers & Miller 1988
//

#include <pbcopper/align/LinearAlignment.h>

#include <cassert>

#include <algorithm>
#include <string>
#include <vector>

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <pbcopper/align/PairwiseAlignment.h>
#include <pbcopper/utility/MinMax.h>

//#define DEBUG_LINEAR_ALIGNMENT

#ifdef DEBUG_LINEAR_ALIGNMENT
#include <iostream>
#endif  // DEBUG_LINEAR_ALIGNMENT

namespace PacBio {
namespace Align {
namespace LinearAlign {

namespace ublas = boost::numeric::ublas;

constexpr int ALIGN_INSERT_SCORE = -2;
constexpr int ALIGN_DELETE_SCORE = -2;
constexpr int ALIGN_MISALIGN_MATCH_SCORE = -1;
constexpr int ALIGN_MATCH_SCORE = +2;

const AlignParams params{ALIGN_MATCH_SCORE, ALIGN_MISALIGN_MATCH_SCORE, ALIGN_INSERT_SCORE,
                         ALIGN_DELETE_SCORE};
const AlignConfig config{params, AlignMode::GLOBAL};

//
// Get transcript of NW alignment taking
//   target[j1..j2] into query[i1..i2] (one-based indexing)
// used for trivial base cases.
//
std::string NWTranscript(const std::string& target, int j1, int j2, const std::string& query,
                         int i1, int i2, int* score)
{
    // If j1 > j2 or i1 > i2, the respective subtarget or subquery is empty,
    // ergo we have pure insertions or deletions.
    assert((i2 - i1 >= -1) && (j2 - j1 >= -1));

    // implement this inline later
    std::string T = target.substr(j1 - 1, j2 - j1 + 1);
    std::string Q = query.substr(i1 - 1, i2 - i1 + 1);
    std::unique_ptr<PacBio::Align::PairwiseAlignment> aln{Align(T, Q, score, config)};
    std::string X = aln->Transcript();
    return X;
}

#ifndef NDEBUG
bool CheckTranscript(const std::string& transcript, const std::string& unalnTarget,
                     const std::string& unalnQuery)
{
    std::unique_ptr<PairwiseAlignment> pa{
        PairwiseAlignment::FromTranscript(transcript, unalnTarget, unalnQuery)};
    return static_cast<bool>(pa);
}
#endif  // NDEBUG

//
// Hirschberg recursion:
// Find optimal transcript taking target[j1..j2] into query[i1..i2] (one-based indices)
// Operates by divide-and-conquer, finding midpoint (m, j*) and recursing on halves, then joining.
// Notes:
//
//    | Alignment  | L                | L_1               | L_2                   |
//    | Path       | (0,0) ~> (I,J)   | (0,0) ~> (m, j*)  | (m, j*) ~> (I, J)     |
//    | T, Q       | T[1..J], Q[1..I] | T[1..j*], Q[1..m] | T[j*+1..J], Q[m+1..I] |
//    | Transcript | X                | X_1               | X_2                   |
//
// target on horizontal, query on vertical
// i refers to query; j refers to target
// this gives better balanced recursion in the (common) semiglobal case
//
std::string OptimalTranscript(const std::string& target, int j1, int j2, const std::string& query,
                              int i1, int i2, ublas::vector<int>& buf1, ublas::vector<int>& buf2,
                              int* score = nullptr)
{
#ifndef NDEBUG
    std::string subtarget = target.substr(j1 - 1, j2 - j1 + 1);
    std::string subquery = query.substr(i1 - 1, i2 - i1 + 1);
#endif

#ifdef DEBUG_LINEAR_ALIGNMENT
    std::cout << "Called OptimalTranscript("
              << "T[" << j1 << ".." << j2 << "], "
              << "Q[" << i1 << ".." << i2 << "])  " << target << ' ' << query << " (" << subtarget
              << ", " << subquery << ")\n";
#endif

    std::string x;
    std::string x1;
    std::string x2;
    int segmentScore;
    const AlignParams& configParams = config.Params;

    //
    // Base case
    //
    if ((j2 - j1 < 1) || (i2 - i1 < 1)) {
        x = NWTranscript(target, j1, j2, query, i1, i2, &segmentScore);
    }

    //
    // Recursive case
    //
    else {
        assert(buf1.size() == target.size() + 1);
        assert(buf2.size() == target.size() + 1);

        ublas::vector<int>& Sm = buf1;  // S-
        ublas::vector<int>& Sp = buf2;  // S+

        int mid = (i1 + i2) / 2;

        //
        // Score forward, i1 upto mid
        // ( T[j1..j2] vs Q[i1..m] )
        //
        Sm(j1 - 1) = 0;
        for (int j = j1; j <= j2; j++) {
            Sm(j) = Sm(j - 1) + configParams.Delete;
        }
        for (int i = i1; i <= mid; i++) {
            int s;
            int c;
            s = Sm(j1 - 1);
            c = Sm(j1 - 1) + configParams.Insert;
            Sm(j1 - 1) = c;
            for (int j = j1; j <= j2; j++) {
                char t = target[j - 1];
                char q = query[i - 1];
                c = Utility::Max(Sm(j) + configParams.Insert,
                                 s + (t == q ? configParams.Match : configParams.Mismatch),
                                 c + configParams.Delete);
                s = Sm(j);
                Sm(j) = c;
            }
        }

        //
        // Score backwards, i2 downto mid
        // ( T[j1..j2] vs Q[m+1..i2] )
        //
        Sp(j2) = 0;
        for (int j = j2 - 1; j >= j1 - 1; j--) {
            Sp(j) = Sp(j + 1) + configParams.Delete;
        }
        for (int i = i2 - 1; i >= mid; i--) {
            int s;
            int c;
            s = Sp(j2);
            c = Sp(j2) + configParams.Delete;
            Sp(j2) = c;
            for (int j = j2 - 1; j >= j1 - 1; j--) {
                char t = target[j];  // j + 1 - 1
                char q = query[i];   // i + 1 - 1
                c = Utility::Max(Sp(j) + configParams.Insert,
                                 s + (t == q ? configParams.Match : configParams.Mismatch),
                                 c + configParams.Delete);
                s = Sp(j);
                Sp(j) = c;
            }
        }

        //
        // Find where optimal path crosses the mid row
        //
        ublas::vector<int> sum = Sm + Sp;
        int j = std::max_element(sum.begin() + j1 - 1, sum.begin() + j2 + 1) - sum.begin();
        segmentScore = sum[j];

        int segment1Score;
        int segment2Score;
        x1 = OptimalTranscript(target, j1, j, query, i1, mid, buf1, buf2, &segment1Score);
        x2 = OptimalTranscript(target, j + 1, j2, query, mid + 1, i2, buf1, buf2, &segment2Score);
        assert(segmentScore == segment1Score + segment2Score);
        x = x1 + x2;
    }

    // Check 1: transcript has to take target[j1..j2] into query[i1..i2]
    assert(CheckTranscript(x, subtarget, subquery));

// Check 2: same score as basic N/W?
#ifndef NDEBUG
    int peerScore;
    std::unique_ptr<PairwiseAlignment> peerAlignment{
        Align(subtarget, subquery, &peerScore, config)};
    assert(peerScore == segmentScore);
#endif

    if (score != nullptr) {
        *score = segmentScore;
    }
    return x;
}

}  // namespace LinearAlign

PairwiseAlignment* AlignLinear(const std::string& target, const std::string& query, int* score,
                               AlignConfig /*unused*/)
{
    using namespace LinearAlign;
    int J = target.length();
    ublas::vector<int> buf1(J + 1);
    ublas::vector<int> buf2(J + 1);
    std::string x =
        OptimalTranscript(target, 1, target.length(), query, 1, query.length(), buf1, buf2, score);
    return PairwiseAlignment::FromTranscript(x, target, query);
}

PairwiseAlignment* AlignLinear(const std::string& target, const std::string& query, AlignConfig cfg)
{
    return AlignLinear(target, query, nullptr, cfg);
}

}  // namespace Align
}  // namespace PacBio

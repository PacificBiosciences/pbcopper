#include <pbcopper/align/AffineAlignment.h>

#include <cassert>
#include <cfloat>

#include <algorithm>
#include <string>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include <pbcopper/align/PairwiseAlignment.h>
#include <pbcopper/utility/MinMax.h>
#include <pbcopper/utility/SequenceUtils.h>

namespace PacBio {
namespace Align {

namespace {

class IupacAware;
class Standard;

inline bool IsIupacPartialMatch(char iupacCode, char b)
{
    assert(iupacCode != b);

    switch (iupacCode) {
        case 'R':
            return (b == 'A' || b == 'G');
        case 'Y':
            return (b == 'C' || b == 'T');
        case 'S':
            return (b == 'G' || b == 'C');
        case 'W':
            return (b == 'A' || b == 'T');
        case 'K':
            return (b == 'G' || b == 'T');
        case 'M':
            return (b == 'A' || b == 'C');
        default:
            return false;
    }
}

template <typename C>
float MatchScore(char t, char q, float matchScore, float mismatchScore, float partialMatchScore);

template <>
float MatchScore<Standard>(char t, char q, float matchScore, float mismatchScore, float /*unused*/)
{
    return (t == q ? matchScore : mismatchScore);
}

template <>
float MatchScore<IupacAware>(char t, char q, float matchScore, float mismatchScore,
                             float partialMatchScore)
{
    if (t == q) {
        return matchScore;
    } else if (IsIupacPartialMatch(t, q) || IsIupacPartialMatch(q, t)) {
        return partialMatchScore;
    } else {
        return mismatchScore;
    }  // NOLINT
}

template <class C>
PairwiseAlignment* AlignAffineGeneric(const std::string& target, const std::string& query,
                                      AffineAlignmentParams params)
{
    // Implementation follows the textbook "two-state" affine gap model
    // description from Durbin et. al
    using boost::numeric::ublas::matrix;

    int I = query.length();
    int J = target.length();
    matrix<float> M(I + 1, J + 1);
    matrix<float> GAP(I + 1, J + 1);

    // Initialization
    M(0, 0) = 0;
    GAP(0, 0) = -FLT_MAX;
    for (int i = 1; i <= I; ++i) {
        M(i, 0) = -FLT_MAX;
        GAP(i, 0) = params.GapOpen + (i - 1) * params.GapExtend;
    }
    for (int j = 1; j <= J; ++j) {
        M(0, j) = -FLT_MAX;
        GAP(0, j) = params.GapOpen + (j - 1) * params.GapExtend;
    }

    // Main part of the recursion
    for (int i = 1; i <= I; ++i) {
        for (int j = 1; j <= J; ++j) {
            float matchScore = MatchScore<C>(target[j - 1], query[i - 1], params.MatchScore,
                                             params.MismatchScore, params.PartialMatchScore);
            M(i, j) = std::max(M(i - 1, j - 1), GAP(i - 1, j - 1)) + matchScore;
            GAP(i, j) =
                Utility::Max(M(i, j - 1) + params.GapOpen, GAP(i, j - 1) + params.GapExtend,
                             M(i - 1, j) + params.GapOpen, GAP(i - 1, j) + params.GapExtend);
        }
    }

    // Perform the traceback
    const int MATCH_MATRIX = 1;
    const int GAP_MATRIX = 2;

    std::string raQuery;
    std::string raTarget;
    int i = I;
    int j = J;
    int mat = (M(I, J) >= GAP(I, J) ? MATCH_MATRIX : GAP_MATRIX);
    int iPrev;
    int jPrev;
    int matPrev;
    while (i > 0 || j > 0) {
        if (mat == MATCH_MATRIX) {
            matPrev = (M(i - 1, j - 1) >= GAP(i - 1, j - 1) ? MATCH_MATRIX : GAP_MATRIX);
            iPrev = i - 1;
            jPrev = j - 1;
            raQuery.push_back(query[iPrev]);
            raTarget.push_back(target[jPrev]);
        } else {
            assert(mat == GAP_MATRIX);
            float s[4];
            s[0] = (j > 0 ? M(i, j - 1) + params.GapOpen : -FLT_MAX);
            s[1] = (j > 0 ? GAP(i, j - 1) + params.GapExtend : -FLT_MAX);
            s[2] = (i > 0 ? M(i - 1, j) + params.GapOpen : -FLT_MAX);
            s[3] = (i > 0 ? GAP(i - 1, j) + params.GapExtend : -FLT_MAX);
            int argMax = std::max_element(s, s + 4) - s;

            matPrev = ((argMax == 0 || argMax == 2) ? MATCH_MATRIX : GAP_MATRIX);
            if (argMax == 0 || argMax == 1) {
                iPrev = i;
                jPrev = j - 1;
                raQuery.push_back('-');
                raTarget.push_back(target[jPrev]);
            } else {
                iPrev = i - 1;
                jPrev = j;
                raQuery.push_back(query[iPrev]);
                raTarget.push_back('-');
            }
        }

        // Go to previous square
        i = iPrev;
        j = jPrev;
        mat = matPrev;
    }

    assert(raQuery.length() == raTarget.length());
    return new PairwiseAlignment(Utility::Reversed(raTarget), Utility::Reversed(raQuery));
}

}  // anonymous namespace

AffineAlignmentParams::AffineAlignmentParams(float matchScore, float mismatchScore, float gapOpen,
                                             float gapExtend, float partialMatchScore)
    : MatchScore(matchScore)
    , MismatchScore(mismatchScore)
    , GapOpen(gapOpen)
    , GapExtend(gapExtend)
    , PartialMatchScore(partialMatchScore)
{
}

AffineAlignmentParams DefaultAffineAlignmentParams() { return {0, -1.0, -1.0, -0.5, 0}; }

AffineAlignmentParams IupacAwareAffineAlignmentParams() { return {0, -1.0, -1.0, -0.5, -0.25}; }

PairwiseAlignment* AlignAffine(const std::string& target, const std::string& query,
                               AffineAlignmentParams params)
{
    return AlignAffineGeneric<Standard>(target, query, params);
}

PairwiseAlignment* AlignAffineIupac(const std::string& target, const std::string& query,
                                    AffineAlignmentParams params)
{
    return AlignAffineGeneric<IupacAware>(target, query, params);
}

}  // namespace Align
}  // namespace PacBio

#include <pbcopper/align/BandedChainAlignment.h>

#include <cassert>
#include <cfloat>

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include <pbcopper/align/internal/BCAlignBlocks.h>
#include <pbcopper/align/internal/BCAlignImpl.h>
#include <pbcopper/utility/MinMax.h>

namespace PacBio {
namespace Align {
namespace {

float score(const char t, const char q, const BandedChainAlignConfig& config)
{
    return (t == q ? config.matchScore_ : config.mismatchPenalty_);
}

void addAlignmentOp(std::string* transcript, const char c) { transcript->push_back(c); }

}  // namespace

namespace Internal {

// ------------------------
// BandedGlobalAlignBlock
// ------------------------

std::string BandedGlobalAlignBlock::Align(const char* target, const char* query, Align::Seed seed)
{

    // ensure horizontal sequence length is >= vertical
    // (simplifies band calculations)
    const std::size_t qLen = seed.EndPositionV() - seed.BeginPositionV();
    const std::size_t tLen = seed.EndPositionH() - seed.BeginPositionH();
    if (qLen == 0) {
        return std::string(tLen, 'D');
    } else if (tLen == 0) {
        return std::string(qLen, 'I');
    }

    std::string result;

    const bool seqsFlipped = (qLen > tLen);
    const char* seq1 =
        (seqsFlipped ? (target + seed.BeginPositionH()) : (query + seed.BeginPositionV()));
    const char* seq2 =
        (seqsFlipped ? (query + seed.BeginPositionV()) : (target + seed.BeginPositionH()));
    const std::size_t seq1Len = (seqsFlipped ? tLen : qLen);
    const std::size_t seq2Len = (seqsFlipped ? qLen : tLen);

    // Initialize space & scores
    Init(seq2Len, seq1Len);

    // for each row
    for (std::size_t i = 1; i <= seq1Len; ++i) {

        // foreach column within band
        const auto& e = lookup_[i];
        for (std::size_t j = e.jBegin_; j <= e.jEnd_; ++j) {

            if (j == 0) {
                continue;
            }

            const auto currentIdx = IndexFor(i, j);
            const auto diagIdx = IndexFor(i - 1, j - 1);
            const auto upIdx = IndexFor(i - 1, j);
            const auto leftIdx = IndexFor(i, j - 1);
            const bool upAllowed = (upIdx != std::string::npos);
            const bool leftAllowed = (leftIdx != std::string::npos);

            const auto s = score(seq2[j - 1], seq1[i - 1], config_);

            matchScores_[currentIdx] = (std::max(matchScores_[diagIdx], gapScores_[diagIdx]) + s);

            gapScores_[currentIdx] = Utility::Max(
                (leftAllowed ? matchScores_[leftIdx] + config_.gapOpenPenalty_ : -FLT_MAX),
                (leftAllowed ? gapScores_[leftIdx] + config_.gapExtendPenalty_ : -FLT_MAX),
                (upAllowed ? matchScores_[upIdx] + config_.gapOpenPenalty_ : -FLT_MAX),
                (upAllowed ? gapScores_[upIdx] + config_.gapExtendPenalty_ : -FLT_MAX));
        }
    }

    // Traceback
    const std::size_t MATCH_MATRIX = 1;
    const std::size_t GAP_MATRIX = 2;

    // find traceback start
    const auto btStart = BacktraceStart(seq2Len, seq1Len);
    std::size_t i = btStart.first;
    std::size_t j = btStart.second;
    const std::size_t backtraceStartIdx = IndexFor(btStart.first, btStart.second);

    std::size_t mat =
        (matchScores_[backtraceStartIdx] >= gapScores_[backtraceStartIdx] ? MATCH_MATRIX
                                                                          : GAP_MATRIX);
    std::size_t iPrev;
    std::size_t jPrev;
    std::size_t matPrev;

    // if not beginning at bottom right)
    if (i < seq1Len) {
        const auto op = (seqsFlipped ? 'D' : 'I');
        for (std::size_t k = seq1Len - i; k > 0; --k) {
            addAlignmentOp(&result, op);
        }
    } else if (j < seq2Len) {
        const auto op = (seqsFlipped ? 'I' : 'D');
        for (std::size_t k = seq2Len - j; k > 0; --k) {
            addAlignmentOp(&result, op);
        }
    }

    while (i > 0 || j > 0) {

        if (mat == MATCH_MATRIX) {

            const auto diagIdx = IndexFor(i - 1, j - 1);
            matPrev = (matchScores_[diagIdx] >= gapScores_[diagIdx] ? MATCH_MATRIX : GAP_MATRIX);
            iPrev = i - 1;
            jPrev = j - 1;
            const auto op = (seq1[iPrev] == seq2[jPrev] ? 'M' : 'R');
            addAlignmentOp(&result, op);

        } else {
            assert(mat == GAP_MATRIX);

            const auto upIdx = IndexFor(i - 1, j);
            const auto leftIdx = IndexFor(i, j - 1);
            const auto upAllowed = (upIdx != std::string::npos);
            const auto leftAllowed = (leftIdx != std::string::npos);

            const std::array<float, 4> s{
                {(j > 0 && leftAllowed ? matchScores_[leftIdx] + config_.gapOpenPenalty_
                                       : -FLT_MAX),
                 (j > 0 && leftAllowed ? gapScores_[leftIdx] + config_.gapExtendPenalty_
                                       : -FLT_MAX),
                 (i > 0 && upAllowed ? matchScores_[upIdx] + config_.gapOpenPenalty_ : -FLT_MAX),
                 (i > 0 && upAllowed ? gapScores_[upIdx] + config_.gapExtendPenalty_ : -FLT_MAX)}};
            const auto argMax = std::distance(s.cbegin(), std::max_element(s.cbegin(), s.cend()));

            matPrev = ((argMax == 0 || argMax == 2) ? MATCH_MATRIX : GAP_MATRIX);
            if (argMax == 0 || argMax == 1) {

                iPrev = i;
                jPrev = j - 1;
                const auto op = (seqsFlipped ? 'I' : 'D');
                addAlignmentOp(&result, op);
            } else {

                iPrev = i - 1;
                jPrev = j;
                const auto op = (seqsFlipped ? 'D' : 'I');
                addAlignmentOp(&result, op);
            }
        }

        // step back one
        i = iPrev;
        j = jPrev;
        mat = matPrev;

        // if at edge, force gap moves from here on
        if (i == 0 || j == 0) {
            mat = GAP_MATRIX;
        }
    }

    // reverse transcript & return
    std::reverse(result.begin(), result.end());
    return result;
}

std::pair<std::size_t, std::size_t> BandedGlobalAlignBlock::BacktraceStart(
    const std::size_t tLen, const std::size_t qLen) const
{
    // NOTE: Finding backtrace start this way allows us to not penalize end-gaps.
    const std::size_t maxIndex = std::min(qLen, tLen);

    // find max score in last column
    std::pair<std::size_t, std::size_t> maxCellRight{maxIndex, maxIndex};
    float maxScoreRight = -FLT_MAX;
    {
        for (std::size_t i = 1; i <= maxIndex; ++i) {
            const auto& e = lookup_[i];
            const std::size_t lastColumn = e.jEnd_;
            const auto idx = IndexFor(i, lastColumn);
            if (matchScores_[idx] > maxScoreRight) {
                maxScoreRight = matchScores_[idx];
                maxCellRight = std::make_pair(i, lastColumn);
            }
        }
    }

    // find max score in last row
    std::pair<std::size_t, std::size_t> maxCellBottom{maxIndex, maxIndex};
    float maxScoreBottom = -FLT_MAX;
    {
        const std::size_t lastRow = maxIndex;
        const auto& lookupElement = lookup_[lastRow];
        for (std::size_t j = lookupElement.jBegin_; j < lookupElement.jEnd_; ++j) {
            const auto idx = IndexFor(lastRow, j);
            if (matchScores_[idx] > maxScoreBottom) {
                maxScoreBottom = matchScores_[idx];
                maxCellBottom = std::make_pair(lastRow, j);
            }
        }
    }

    return (maxScoreBottom > maxScoreRight ? maxCellBottom : maxCellRight);
}

std::size_t BandedGlobalAlignBlock::IndexFor(const std::size_t i, const std::size_t j) const
{
    // if in matrix
    if (i != std::string::npos && j != std::string::npos) {
        const auto& e = lookup_[i];
        // hand array index back to caller for (i,j) if in-band
        if (j >= e.jBegin_ && j <= e.jEnd_) {
            return e.arrayStart_ + (j - e.jBegin_);
        }
    }

    // (i,j) either out of matrix bounds, or out-of-band
    return std::string::npos;
}

void BandedGlobalAlignBlock::Init(const std::size_t tLen, const std::size_t qLen)
{
    const auto numElements = InitLookup(tLen, qLen);
    InitScores(tLen, qLen, numElements);
}

std::size_t BandedGlobalAlignBlock::InitLookup(const std::size_t tLen, const std::size_t qLen)
{
    // ensure space
    lookup_.clear();
    lookup_.reserve(qLen + 1);

    const auto t = static_cast<std::int64_t>(tLen);
    const auto q = static_cast<std::int64_t>(qLen);
    const auto k = static_cast<std::int64_t>(config_.bandExtend_);
    assert(t >= q);

    std::size_t arrayStart = 0;
    std::int64_t jBegin = 0;
    std::int64_t jEnd = 0;
    for (std::int64_t i = 0; i < q + 1; ++i) {

        // jBegin
        jBegin = i - k;
        if (jBegin < 0) {
            jBegin = 0;
        }

        // jEnd
        jEnd = i + k;
        if (jEnd > t) {
            jEnd = t;
        }

        // sanity checks
        assert(jBegin >= 0);
        assert(jEnd >= 0);
        assert(jEnd >= jBegin);

        // store lookup values
        lookup_.emplace_back(arrayStart, static_cast<std::size_t>(jBegin),
                             static_cast<std::size_t>(jEnd));

        // update arrayStart for next row (or 'numElements' on exit)
        const auto rowElements = static_cast<std::size_t>(jEnd - jBegin) + 1;
        arrayStart += (rowElements);
    }

    return arrayStart;
}

void BandedGlobalAlignBlock::InitScores(const std::size_t tLen, const std::size_t qLen,
                                        const std::size_t n)
{
    matchScores_.resize(n);
    gapScores_.resize(n);

    matchScores_[0] = 0;
    gapScores_[0] = -FLT_MAX;

    const auto maxQ = std::min(qLen, config_.bandExtend_);
    const auto maxT = std::min(tLen, config_.bandExtend_);

    for (std::size_t i = 1; i <= maxQ; ++i) {
        const auto idx = IndexFor(i, 0);
        matchScores_[idx] = -FLT_MAX;
        gapScores_[idx] = config_.gapOpenPenalty_ + (i - 1) * config_.gapExtendPenalty_;
    }

    for (std::size_t j = 1; j <= maxT; ++j) {
        const auto idx = IndexFor(0, j);
        matchScores_[idx] = -FLT_MAX;
        gapScores_[idx] = config_.gapOpenPenalty_ + (j - 1) * config_.gapExtendPenalty_;
    }
}

// --------------------------
// StandardGlobalAlignBlock
// --------------------------

std::string StandardGlobalAlignBlock::Align(const char* target, const std::size_t tLen,
                                            const char* query, const std::size_t qLen)
{
    // Initialize space & scores
    Init(tLen, qLen);

    // Main loop
    for (std::size_t i = 1; i <= qLen; ++i) {
        for (std::size_t j = 1; j <= tLen; ++j) {
            const auto s = score(target[j - 1], query[i - 1], config_);
            matchScores_[i][j] =
                (std::max(matchScores_[i - 1][j - 1], gapScores_[i - 1][j - 1]) + s);
            gapScores_[i][j] = Utility::Max(matchScores_[i][j - 1] + config_.gapOpenPenalty_,
                                            gapScores_[i][j - 1] + config_.gapExtendPenalty_,
                                            matchScores_[i - 1][j] + config_.gapOpenPenalty_,
                                            gapScores_[i - 1][j] + config_.gapExtendPenalty_);
        }
    }

    // Traceback
    const std::size_t MATCH_MATRIX = 1;
    const std::size_t GAP_MATRIX = 2;

    // find traceback start
    const auto btStart = BacktraceStart(tLen, qLen);
    std::size_t i = btStart.first;
    std::size_t j = btStart.second;
    std::size_t mat =
        (matchScores_[btStart.first][btStart.second] >= gapScores_[btStart.first][btStart.second]
             ? MATCH_MATRIX
             : GAP_MATRIX);
    std::size_t iPrev;
    std::size_t jPrev;
    std::size_t matPrev;

    std::string result;

    // if not beginning at bottom right, add corresponding indel
    if (i < qLen) {
        for (std::size_t k = qLen - i; k > 0; --k) {
            addAlignmentOp(&result, 'I');
        }
    } else if (j < tLen) {
        for (std::size_t k = tLen - j; k > 0; --k) {
            addAlignmentOp(&result, 'D');
        }
    }

    // traceback remaining sequence
    while (i > 0 || j > 0) {

        if (mat == MATCH_MATRIX) {
            matPrev = (matchScores_[i - 1][j - 1] >= gapScores_[i - 1][j - 1] ? MATCH_MATRIX
                                                                              : GAP_MATRIX);
            iPrev = i - 1;
            jPrev = j - 1;
            const auto op = (query[iPrev] == target[jPrev] ? 'M' : 'R');
            addAlignmentOp(&result, op);

        } else {
            assert(mat == GAP_MATRIX);

            const std::array<float, 4> s{
                {(j > 0 ? matchScores_[i][j - 1] + config_.gapOpenPenalty_ : -FLT_MAX),
                 (j > 0 ? gapScores_[i][j - 1] + config_.gapExtendPenalty_ : -FLT_MAX),
                 (i > 0 ? matchScores_[i - 1][j] + config_.gapOpenPenalty_ : -FLT_MAX),
                 (i > 0 ? gapScores_[i - 1][j] + config_.gapExtendPenalty_ : -FLT_MAX)}};
            const auto argMax = std::distance(s.cbegin(), std::max_element(s.cbegin(), s.cend()));

            matPrev = ((argMax == 0 || argMax == 2) ? MATCH_MATRIX : GAP_MATRIX);
            if (argMax == 0 || argMax == 1) {
                iPrev = i;
                jPrev = j - 1;
                addAlignmentOp(&result, 'D');
            } else {
                iPrev = i - 1;
                jPrev = j;
                addAlignmentOp(&result, 'I');
            }
        }

        // step back one
        i = iPrev;
        j = jPrev;
        mat = matPrev;
    }

    // reverse tanscript & return
    std::reverse(result.begin(), result.end());
    return result;
}

std::pair<std::size_t, std::size_t> StandardGlobalAlignBlock::BacktraceStart(
    const std::size_t tLen, const std::size_t qLen) const
{
    // NOTE: Finding backtrace start this way allows us to not penalize end-gaps.

    // find max score in last column
    std::pair<std::size_t, std::size_t> maxCellRight{qLen, tLen};
    float maxScoreRight = -FLT_MAX;
    const std::size_t lastColumn = tLen;
    for (std::size_t i = 1; i <= qLen; ++i) {
        if (matchScores_[i][lastColumn] > maxScoreRight) {
            maxScoreRight = matchScores_[i][lastColumn];
            maxCellRight = std::make_pair(i, lastColumn);
        }
    }

    // find max score in last row
    std::pair<std::size_t, std::size_t> maxCellBottom{qLen, tLen};
    float maxScoreBottom = -FLT_MAX;
    const std::size_t lastRow = qLen;
    for (std::size_t j = 1; j <= tLen; ++j) {
        if (matchScores_[lastRow][j] > maxScoreBottom) {
            maxScoreBottom = matchScores_[lastRow][j];
            maxCellBottom = std::make_pair(lastRow, j);
        }
    }

    return (maxScoreBottom > maxScoreRight ? maxCellBottom : maxCellRight);
}

void StandardGlobalAlignBlock::Init(const std::size_t tLen, const std::size_t qLen)
{
    // ensure space
    matchScores_.resize(qLen + 1);
    gapScores_.resize(qLen + 1);

    assert(matchScores_.size() == qLen + 1);
    assert(gapScores_.size() == qLen + 1);

    for (std::size_t i = 0; i < qLen + 1; ++i) {
        matchScores_[i].resize(tLen + 1);
        gapScores_[i].resize(tLen + 1);
    }

    // fill out initial scores
    matchScores_[0][0] = 0;
    gapScores_[0][0] = -FLT_MAX;
    for (std::size_t i = 1; i <= qLen; ++i) {
        matchScores_[i][0] = -FLT_MAX;
        gapScores_[i][0] = config_.gapOpenPenalty_ + (i - 1) * config_.gapExtendPenalty_;
    }
    for (std::size_t j = 1; j <= tLen; ++j) {
        matchScores_[0][j] = -FLT_MAX;
        gapScores_[0][j] = config_.gapOpenPenalty_ + (j - 1) * config_.gapExtendPenalty_;
    }
}

// ------------------------
// BandedChainAlignerImpl
// ------------------------

static std::vector<Align::Seed>::const_iterator FirstAnchorSeed(
    const std::vector<Align::Seed>& seeds, const std::size_t band)
{
    return std::find_if(seeds.cbegin(), seeds.cend(), [band](const Align::Seed& seed) {
        return seed.BeginPositionH() >= band && seed.BeginPositionV() >= band;
    });
}

static std::vector<Align::Seed>::const_iterator LastAnchorSeed(
    const std::vector<Align::Seed>& seeds, const std::size_t tLen, const std::size_t qLen,
    const std::size_t band)
{
    return std::find_if(seeds.crbegin(), seeds.crend(),
                        [tLen, qLen, band](const Align::Seed& seed) {
                            return seed.EndPositionH() + band < tLen &&
                                   seed.EndPositionV() + band < qLen;
                        })
        .base();  // convert reverse_iterator -> iterator
}

BandedChainAlignerImpl::BandedChainAlignerImpl(const BandedChainAlignConfig& config)
    : config_(config), gapBlock_(config), seedBlock_(config), gapBlockBeginH_(0), gapBlockBeginV_(0)
{}

BandedChainAlignment BandedChainAlignerImpl::Align(const char* target, const std::size_t targetLen,
                                                   const char* query, const std::size_t queryLen,
                                                   const std::vector<Align::Seed>& seeds)
{
    // return empty alignment on empty seeds
    if (seeds.empty()) {
        return BandedChainAlignment{};
    }

    // reset state & store input sequence info
    Initialize(target, targetLen, query, queryLen);

    // step through merged seeds (all overlaps collapsed)
    //   1 - align gap region before current seed, and then
    //   2 - align current seed
    const auto mergedSeeds = MergeSeeds(seeds);
    const auto band = config_.bandExtend_;
    auto it = FirstAnchorSeed(mergedSeeds, band);
    const auto end = LastAnchorSeed(mergedSeeds, targetLen, queryLen, band);
    for (; it != end; ++it) {
        AlignGapBlock(*it);
        AlignSeedBlock(*it);
    }

    // finally align last gap region after last seed & return result
    AlignLastGapBlock();
    return Result();
}

void BandedChainAlignerImpl::AlignGapBlock(const std::size_t hLength, const std::size_t vLength)
{
    // do 'standard' DP align
    auto transcript = gapBlock_.Align(sequences_.target + gapBlockBeginH_, hLength,
                                      sequences_.query + gapBlockBeginV_, vLength);

    // incorporate alignment into total result
    StitchTranscripts(&globalTranscript_, std::move(transcript));
}

void BandedChainAlignerImpl::AlignGapBlock(const Align::Seed& nextSeed)
{
    const std::size_t hLength = nextSeed.BeginPositionH() - gapBlockBeginH_;
    const std::size_t vLength = nextSeed.BeginPositionV() - gapBlockBeginV_;
    AlignGapBlock(hLength, vLength);
}

void BandedChainAlignerImpl::AlignLastGapBlock()
{
    const std::size_t hLength = sequences_.targetLen - gapBlockBeginH_;
    const std::size_t vLength = sequences_.queryLen - gapBlockBeginV_;
    AlignGapBlock(hLength, vLength);
}

void BandedChainAlignerImpl::AlignSeedBlock(const Align::Seed& seed)
{
    // do seed-guided, banded align
    auto transcript = seedBlock_.Align(sequences_.target, sequences_.query, seed);

    // incorporate alignment into total result
    StitchTranscripts(&globalTranscript_, std::move(transcript));

    // see if we ended with an indel, if so remove that and try re-aligning that
    // portion in the next alignment phase
    std::size_t hOffset = 0;
    std::size_t vOffset = 0;
    const auto& lastOp = globalTranscript_.back();
    if (lastOp == 'D') {
        while (globalTranscript_.back() == 'D') {
            ++hOffset;
            globalTranscript_.pop_back();
        }
    } else if (lastOp == 'I') {
        while (globalTranscript_.back() == 'I') {
            ++vOffset;
            globalTranscript_.pop_back();
        }
    }

    // update offsets for next alignment block
    gapBlockBeginH_ = seed.EndPositionH() - hOffset;
    gapBlockBeginV_ = seed.EndPositionV() - vOffset;
}

void BandedChainAlignerImpl::Initialize(const char* target, const std::size_t targetLen,
                                        const char* query, const std::size_t queryLen)
{
    globalTranscript_.clear();
    globalScore_ = std::numeric_limits<std::int64_t>::min();
    gapBlockBeginH_ = 0;
    gapBlockBeginV_ = 0;

    sequences_ = Sequences{target, targetLen, query, queryLen};
}

std::vector<Align::Seed> BandedChainAlignerImpl::MergeSeeds(const std::vector<Align::Seed>& seeds)
{
    // no merging needed on empty or single-element containers
    if (seeds.size() <= 1) {
        return seeds;
    }

    // push first seed into output container
    std::vector<Align::Seed> mergedSeeds;
    mergedSeeds.reserve(seeds.size());
    mergedSeeds.push_back(seeds.front());
    auto currentSeed = mergedSeeds.begin();

    // iterate over remaining seeds
    auto inputIter = seeds.cbegin() + 1;
    auto inputEnd = seeds.cend();
    for (; inputIter != inputEnd; ++inputIter) {

        // if input seed starts after current seed
        if (inputIter->BeginPositionH() > currentSeed->EndPositionH() &&
            inputIter->BeginPositionV() > currentSeed->EndPositionV()) {
            mergedSeeds.push_back(*inputIter);
            currentSeed = mergedSeeds.end() - 1;
        }

        // else contiguous or overlapping
        else {
            currentSeed->BeginPositionH(
                std::min(inputIter->BeginPositionH(), currentSeed->BeginPositionH()));
            currentSeed->EndPositionH(
                std::max(inputIter->EndPositionH(), currentSeed->EndPositionH()));
            currentSeed->BeginPositionV(
                std::min(inputIter->BeginPositionV(), currentSeed->BeginPositionV()));
            currentSeed->EndPositionV(
                std::max(inputIter->EndPositionV(), currentSeed->EndPositionV()));
        }
    }
    return mergedSeeds;
}

BandedChainAlignment BandedChainAlignerImpl::Result()
{
    return BandedChainAlignment{config_,          sequences_.target,   sequences_.targetLen,
                                sequences_.query, sequences_.queryLen, globalTranscript_};
}

void BandedChainAlignerImpl::StitchTranscripts(std::string* global, std::string&& local)
{
    assert(global);

    // quick checks if either transcript is empty
    if (local.empty()) {
        return;
    }
    if (global->empty()) {
        *global = std::move(local);
        return;
    }

    global->reserve(global->size() + local.size());
    *global += local;
}

}  // namespace Internal

// ------------------------
// BandedChainAlignment
// ------------------------

BandedChainAlignment::BandedChainAlignment(const BandedChainAlignConfig& config, std::string target,
                                           std::string query, const std::string& transcript)
    : config_(config)
    , target_{std::move(target)}
    , query_{std::move(query)}
    , transcript_{std::move(transcript)}
{
    alignedTarget_.reserve(target_.size());
    alignedQuery_.reserve(query_.size());

    enum class TranscriptState : uint8_t
    {
        Match_Mismatch,
        Insertion,
        Deletion
    };

    score_ = 0;
    TranscriptState state = TranscriptState::Match_Mismatch;
    std::size_t tPos = 0;
    std::size_t qPos = 0;
    for (const auto& c : transcript_) {
        switch (c) {
            case 'M': {
                // update aligned seqs
                alignedQuery_.push_back(query_[qPos]);
                ++qPos;
                alignedTarget_.push_back(target_[tPos]);
                ++tPos;

                // update score & current state
                score_ += config_.matchScore_;
                state = TranscriptState::Match_Mismatch;
                break;
            }
            case 'R': {
                // update aligned seqs
                alignedQuery_.push_back(query_[qPos]);
                ++qPos;
                alignedTarget_.push_back(target_[tPos]);
                ++tPos;

                // update score & current state
                score_ += config_.mismatchPenalty_;
                state = TranscriptState::Match_Mismatch;
                break;
            }
            case 'I': {
                // update aligned seqs
                alignedQuery_.push_back(query_[qPos]);
                ++qPos;
                alignedTarget_.push_back('-');

                // update score (do not penalize end gaps) & current state
                if (tPos != 0 && tPos < target_.size()) {
                    if (state != TranscriptState::Insertion) {
                        score_ += config_.gapOpenPenalty_;
                    } else {
                        score_ += config_.gapExtendPenalty_;
                    }
                }
                state = TranscriptState::Insertion;
                break;
            }
            case 'D': {
                // update aligned seqs
                alignedQuery_.push_back('-');
                alignedTarget_.push_back(target_[tPos]);
                ++tPos;

                // update score (do not penalize end gaps) & current state
                if (qPos != 0 && qPos < query_.size()) {
                    if (state != TranscriptState::Deletion) {
                        score_ += config_.gapOpenPenalty_;
                    } else {
                        score_ += config_.gapExtendPenalty_;
                    }
                }
                state = TranscriptState::Deletion;
                break;
            }
            default:
                throw std::runtime_error{
                    "[pbcopper] banded chain alignment ERROR: unknown transcript code: " +
                    std::string(1, c)};
        }
    }
}

BandedChainAlignment::BandedChainAlignment(const BandedChainAlignConfig& config, const char* target,
                                           const std::size_t targetLen, const char* query,
                                           const std::size_t queryLen,
                                           const std::string& transcript)
    : BandedChainAlignment(config, std::string{target, targetLen}, std::string{query, queryLen},
                           transcript)
{}

float BandedChainAlignment::Identity() const
{
    assert(alignedQuery_.length() == alignedTarget_.length());

    std::size_t numMatches = 0;
    const std::size_t len = alignedQuery_.length();
    for (std::size_t i = 0; i < len; ++i) {
        if (alignedQuery_[i] == alignedTarget_[i]) {
            ++numMatches;
        }
    }
    return (100.0F * static_cast<float>(numMatches) / len);
}

int64_t BandedChainAlignment::Score() const { return score_; }

// ------------------------
// BandedChainAlignConfig
// ------------------------

BandedChainAlignConfig BandedChainAlignConfig::Default()
{
    // match, mismatch, gapOpen, gapExtend, band
    return BandedChainAlignConfig{2.0F, -1.0F, -2.0F, -1.0F, 15};
}

// --------------------------
// alignment free functions
// --------------------------

BandedChainAlignment BandedChainAlign(const char* target, const std::size_t targetLen,
                                      const char* query, const std::size_t queryLen,
                                      const std::vector<Align::Seed>& seeds,
                                      const BandedChainAlignConfig& config)
{
    Internal::BandedChainAlignerImpl impl{config};
    return impl.Align(target, targetLen, query, queryLen, seeds);
}

BandedChainAlignment BandedChainAlign(const std::string& target, const std::string& query,
                                      const std::vector<PacBio::Align::Seed>& seeds,
                                      const BandedChainAlignConfig& config)
{
    return BandedChainAlign(target.c_str(), target.size(), query.c_str(), query.size(), seeds,
                            config);
}

}  // namespace Align
}  // namespace PacBio

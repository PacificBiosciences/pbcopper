#include <pbcopper/data/Cigar.h>

#include <pbcopper/utility/SequenceUtils.h>
#include <pbcopper/utility/Ssize.h>

#include <iterator>
#include <ostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <type_traits>

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Data {

Cigar::Cigar(const char* str) : std::vector<CigarOperation>{}
{
    std::size_t numberStart = 0;
    std::size_t i = 0;
    for (const char* c = str; *c; ++i, ++c) {
        if (!std::isdigit(*c)) {
            const std::size_t distance = i - numberStart;
            const uint32_t length = std::stoul(std::string{str, numberStart, distance});
            push_back(CigarOperation(*c, length));
            numberStart = i + 1;
        }
    }
}

Cigar::Cigar(const std::string& cigarString) : Cigar{cigarString.c_str()} {}

Cigar::Cigar(std::vector<CigarOperation> cigar) : std::vector<CigarOperation>{std::move(cigar)} {}

Cigar Cigar::FromStdString(const std::string& stdString) { return Cigar(stdString.c_str()); }

Cigar Cigar::FromCStr(const char* str) { return Cigar(str); }

std::string Cigar::ToStdString() const
{
    std::ostringstream s;
    const auto endIt = this->cend();
    for (auto iter = this->cbegin(); iter != endIt; ++iter) {
        const CigarOperation& cigar = (*iter);
        s << cigar.Length() << cigar.Char();
    }
    return s.str();
}

std::size_t ReferenceLength(const Cigar& cigar)
{
    std::size_t length = 0;
    for (const auto& op : cigar) {
        if (ConsumesReference(op.Type())) {
            length += op.Length();
        }
    }
    return length;
}

std::ostream& operator<<(std::ostream& os, const Cigar& cigar)
{
    return os << "Cigar(" << cigar.ToStdString() << ')';
}

/// http://bitbucket.pacificbiosciences.com:7990/projects/SAT/repos/pbmm2/browse/src/MM2Helper.cpp#980
CigarBaseCounts CigarOpsCalculator(const Cigar& cigar)
{

    CigarBaseCounts results = {};

    for (const auto& c : cigar) {
        int32_t len = c.Length();
        switch (c.Type()) {
            case Data::CigarOperationType::INSERTION:
                results.InsertionBases += len;
                ++results.InsertionEvents;
                break;
            case Data::CigarOperationType::DELETION:
                results.DeletionBases += len;
                ++results.DeletionEvents;
                break;
            case Data::CigarOperationType::SEQUENCE_MISMATCH:
                results.MismatchBases += len;
                ++results.MismatchEvents;
                break;
            case Data::CigarOperationType::REFERENCE_SKIP:
                break;
            case Data::CigarOperationType::SEQUENCE_MATCH:
            case Data::CigarOperationType::ALIGNMENT_MATCH:
                results.MatchBases += len;
                ++results.MatchEvents;
                break;
            case Data::CigarOperationType::PADDING:
            case Data::CigarOperationType::SOFT_CLIP:
            case Data::CigarOperationType::HARD_CLIP:
                break;
            case Data::CigarOperationType::UNKNOWN_OP:
            default:
                throw std::runtime_error("UNKNOWN CIGAR OPERATION");
                break;
        }
    }

    results.NumAlignedBases = results.MatchBases + results.InsertionBases + results.MismatchBases;
    results.Identity = 100.0 * results.MatchBases /
                       (results.MatchBases + results.MismatchBases + results.DeletionBases +
                        results.InsertionBases);
    results.GapCompressedIdentity = 100.0 * results.MatchBases /
                                    (results.MatchBases + results.MismatchBases +
                                     results.DeletionEvents + results.InsertionEvents);

    return results;
}

bool ConvertCigarToM5(const std::string& ref, const std::string& query, bool qReversed,
                      const Data::Cigar& cigar, std::string& retRefAln, std::string& retQueryAln)
{
    return ConvertCigarToM5(ref, query, 0, ref.size(), 0, query.size(), qReversed, cigar, retRefAln,
                            retQueryAln);
}

bool ConvertCigarToM5(const std::string& ref, const std::string& query, const int32_t rStart,
                      const int32_t rEnd, const int32_t qStart, const int32_t qEnd,
                      const bool qReversed, const Data::Cigar& cigar, std::string& retRefAln,
                      std::string& retQueryAln)
{
    std::ostringstream rss;
    std::ostringstream qss;

    retRefAln.clear();
    retQueryAln.clear();

    // Calculate the query and reference length from the CIGAR
    // to check for sanity.
    int32_t calcRefLen = 0;
    int32_t calcQueryLen = 0;
    for (const auto& cigarOp : cigar) {
        const auto op = cigarOp.Type();
        switch (op) {
            case CigarOperationType::ALIGNMENT_MATCH:
            case CigarOperationType::SEQUENCE_MATCH:
            case CigarOperationType::SEQUENCE_MISMATCH: {
                calcRefLen += cigarOp.Length();
                calcQueryLen += cigarOp.Length();
                break;
            }
            case Data::CigarOperationType::INSERTION:
            case CigarOperationType::SOFT_CLIP: {
                calcQueryLen += cigarOp.Length();
                break;
            }
            case CigarOperationType::DELETION:
            case CigarOperationType::REFERENCE_SKIP: {
                calcRefLen += cigarOp.Length();
                break;
            }
            default: {
                std::string msg{"[pbcopper] CIGAR conversion ERROR: unsupported op: '"};
                msg += cigarOp.Char();
                msg += "'";
                throw std::runtime_error{msg};
            }
        }
    }
    if (cigar.empty() || calcQueryLen != (qEnd - qStart) || calcRefLen != (rEnd - rStart)) {
        return false;
    }

    // Prepare the query for simpler usage.
    std::string querySub = query.substr(qStart, qEnd - qStart);
    if (qReversed) {
        querySub = Utility::ReverseComplemented(querySub);
    }

    int32_t qPos = 0;  // We extracted the entire subsequence.
    int32_t rPos = rStart;

    for (auto& cigarOp : cigar) {
        auto op = cigarOp.Type();
        auto count = cigarOp.Length();
        switch (op) {
            case CigarOperationType::ALIGNMENT_MATCH:
            case CigarOperationType::SEQUENCE_MATCH:
            case CigarOperationType::SEQUENCE_MISMATCH: {
                qss << querySub.substr(qPos, count);
                rss << ref.substr(rPos, count);
                qPos += count;
                rPos += count;
                break;
            }
            case CigarOperationType::INSERTION:
            case CigarOperationType::SOFT_CLIP: {
                qss << querySub.substr(qPos, count);
                rss << std::string(count, '-');
                qPos += count;
                break;
            }
            case CigarOperationType::DELETION:
            case CigarOperationType::REFERENCE_SKIP: {
                qss << std::string(count, '-');
                rss << ref.substr(rPos, count);
                rPos += count;
                break;
            }
            default: {
                std::string msg{"[pbcopper] CIGAR conversion ERROR: unsupported op: '"};
                msg += cigarOp.Char();
                msg += "'";
                throw std::runtime_error{msg};
            }
        }
    }

    retRefAln = rss.str();
    retQueryAln = qss.str();
    return true;
}

bool ValidateCigar(const Data::Cigar& cigar, const std::string_view ref,
                   const std::string_view query)
{
    const auto cigarBegin = std::cbegin(cigar);
    const auto cigarEnd = std::cend(cigar);

    const int32_t refLen = Utility::Ssize(ref);
    const int32_t queryLen = Utility::Ssize(query);

    int32_t refPos = 0;
    int32_t queryPos = 0;

    for (auto it = cigarBegin; it != cigarEnd; ++it) {
        const Data::CigarOperationType type = it->Type();
        const int32_t length = it->Length();

        if ((it != cigarBegin) && (type == std::prev(it)->Type())) {
            return false;
        }

        if (type == Data::CigarOperationType::INSERTION) {
            queryPos += length;

        } else if ((type == Data::CigarOperationType::DELETION) ||
                   (type == Data::CigarOperationType::REFERENCE_SKIP)) {
            refPos += length;

        } else if (type == Data::CigarOperationType::SOFT_CLIP) {
            if ((it != cigarBegin) && (it != std::prev(cigarEnd)) &&
                (std::next(it)->Type() != Data::CigarOperationType::HARD_CLIP) &&
                (std::prev(it)->Type() != Data::CigarOperationType::HARD_CLIP)) {
                return false;
            }
            queryPos += length;

        } else if (type == Data::CigarOperationType::HARD_CLIP) {
            if ((it != cigarBegin) && (it != std::prev(cigarEnd))) {
                return false;
            }

        } else if (type == Data::CigarOperationType::PADDING) {
            continue;

        } else if (type == Data::CigarOperationType::SEQUENCE_MATCH) {
            for (int32_t i = 0; i < length; ++i, ++refPos, ++queryPos) {
                if ((refPos >= refLen) || (queryPos >= queryLen) ||
                    (ref[refPos] != query[queryPos])) {
                    return false;
                }
            }

        } else if (type == Data::CigarOperationType::SEQUENCE_MISMATCH) {
            for (int32_t i = 0; i < length; ++i, ++refPos, ++queryPos) {
                if ((refPos >= refLen) || (queryPos >= queryLen) ||
                    (ref[refPos] == query[queryPos])) {
                    return false;
                }
            }

        } else {  // ALIGNMENT_MATCH | UNKNOWN_OP
            return false;
        }
    }

    return (refPos == refLen) && (queryPos == queryLen);
}

Data::Cigar LeftAlignCigar(const Data::Cigar& cigar, const std::string_view ref,
                           const std::string_view query, const bool replaceMismatches)
{
    assert(ValidateCigar(cigar, ref, query));

    std::stack<Data::CigarOperation> cigarStack;

    int32_t refBegin = 0;
    int32_t refEnd = Utility::Ssize(ref);
    int32_t refLoan = 0;

    int32_t queryBegin = 0;
    int32_t queryEnd = Utility::Ssize(query);
    int32_t queryLoan = 0;

    Data::Cigar result;
    result.reserve(std::size(cigar) * 1.5);

    // helper lambdas
    const auto updateCigarStack = [&]() {
        while (!cigarStack.empty()) {
            Data::CigarOperation op = cigarStack.top();
            const Data::CigarOperationType type = op.Type();

            assert(type != Data::CigarOperationType::PADDING);
            assert(type != Data::CigarOperationType::HARD_CLIP);
            assert(type != Data::CigarOperationType::SOFT_CLIP);

            if (((queryLoan == 0) && (refLoan == 0)) ||
                ((type == Data::CigarOperationType::INSERTION) && (queryLoan == 0)) ||
                ((type == Data::CigarOperationType::DELETION) && (refLoan == 0))) {
                break;
            }

            cigarStack.pop();
            int32_t length = op.Length();

            if ((type == Data::CigarOperationType::SEQUENCE_MATCH) ||
                (type == Data::CigarOperationType::SEQUENCE_MISMATCH)) {
                const int32_t loan = std::min(queryLoan, refLoan);
                if (loan >= length) {
                    queryLoan -= length;
                    refLoan -= length;
                } else {
                    queryLoan -= loan;
                    refLoan -= loan;
                    length -= loan;
                    if ((queryLoan > 0) && (refLoan == 0)) {
                        if (queryLoan >= length) {
                            cigarStack.emplace(Data::CigarOperationType::DELETION, length);
                            queryLoan -= length;
                        } else {
                            cigarStack.emplace(type, length - queryLoan);
                            cigarStack.emplace(Data::CigarOperationType::DELETION, queryLoan);
                            queryLoan = 0;
                        }
                    } else if ((queryLoan == 0) && (refLoan > 0)) {
                        if (refLoan >= length) {
                            cigarStack.emplace(Data::CigarOperationType::INSERTION, length);
                            refLoan -= length;
                        } else {
                            cigarStack.emplace(type, length - refLoan);
                            cigarStack.emplace(Data::CigarOperationType::INSERTION, refLoan);
                            refLoan = 0;
                        }
                    }
                }

            } else if (type == Data::CigarOperationType::INSERTION) {
                if (queryLoan >= length) {
                    queryLoan -= length;
                } else {
                    length -= queryLoan;
                    queryLoan = 0;
                    cigarStack.emplace(Data::CigarOperationType::INSERTION, length);
                }

            } else if ((type == Data::CigarOperationType::DELETION) ||
                       (type == Data::CigarOperationType::REFERENCE_SKIP)) {
                if (refLoan >= length) {
                    refLoan -= length;
                } else {
                    length -= refLoan;
                    refLoan = 0;
                    cigarStack.emplace(Data::CigarOperationType::DELETION, length);
                }
            }
        }
    };

    const auto updateOrAppendToResult = [&result](Data::CigarOperation op) {
        if (!result.empty() && (result.back().Type() == op.Type())) {
            result.back().Length(result.back().Length() + op.Length());
        } else {
            result.emplace_back(op);
        }
    };

    // preprocess query
    if (!cigar.empty() && (cigar.front().Type() == Data::CigarOperationType::SOFT_CLIP)) {
        queryBegin += cigar.front().Length();
    }
    if ((Utility::Ssize(cigar) > 1) &&
        (cigar.back().Type() == Data::CigarOperationType::SOFT_CLIP)) {
        queryEnd -= cigar.back().Length();
    }

    // unroll
    for (const auto& it : cigar) {
        Data::CigarOperationType type = it.Type();

        if (type == Data::CigarOperationType::PADDING) {
            continue;
        }
        if ((type == Data::CigarOperationType::HARD_CLIP) ||
            (type == Data::CigarOperationType::SOFT_CLIP)) {
            // SOFT_CLIP does not consume query due to preprocess
            result.emplace_back(it);
            continue;
        }

        if (replaceMismatches && (type == Data::CigarOperationType::SEQUENCE_MISMATCH)) {
            const int32_t length = it.Length();
            for (int32_t i = 0; i < length; ++i) {
                cigarStack.emplace(Data::CigarOperationType::INSERTION, 1);
                cigarStack.emplace(Data::CigarOperationType::DELETION, 1);
            }
        } else {
            cigarStack.emplace(it);
        }

        updateCigarStack();
        while (!cigarStack.empty()) {
            Data::CigarOperation op = cigarStack.top();
            cigarStack.pop();

            type = op.Type();
            const int32_t length = op.Length();

            if ((type == Data::CigarOperationType::SEQUENCE_MATCH) ||
                (type == Data::CigarOperationType::SEQUENCE_MISMATCH)) {
                updateOrAppendToResult(op);
                refBegin += length;
                queryBegin += length;

            } else if (type == Data::CigarOperationType::INSERTION) {
                for (int32_t i = 0; i < length; ++i, ++queryBegin) {
                    if ((refBegin < refEnd) && (ref[refBegin] == query[queryBegin])) {
                        // left align
                        updateOrAppendToResult(
                            Data::CigarOperation{Data::CigarOperationType::SEQUENCE_MATCH, 1});
                        ++refBegin;
                        ++refLoan;
                    } else {
                        updateOrAppendToResult(
                            Data::CigarOperation{Data::CigarOperationType::INSERTION, 1});
                    }
                }

            } else if ((type == Data::CigarOperationType::DELETION) ||
                       (type == Data::CigarOperationType::REFERENCE_SKIP)) {
                for (int32_t i = 0; i < length; ++i, ++refBegin) {
                    if ((queryBegin < queryEnd) && (ref[refBegin] == query[queryBegin])) {
                        // left align
                        updateOrAppendToResult(
                            Data::CigarOperation{Data::CigarOperationType::SEQUENCE_MATCH, 1});
                        ++queryBegin;
                        ++queryLoan;
                    } else {
                        updateOrAppendToResult(
                            Data::CigarOperation{Data::CigarOperationType::DELETION, 1});
                    }
                }
            }

            updateCigarStack();
        }
    }

    assert(ValidateCigar(result, ref, query));

    return result;
}

}  // namespace Data
}  // namespace PacBio

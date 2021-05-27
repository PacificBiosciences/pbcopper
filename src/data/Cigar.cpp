#include <pbcopper/data/Cigar.h>

#include <cstddef>
#include <cstdint>

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include <pbcopper/utility/SequenceUtils.h>

namespace PacBio {
namespace Data {

Cigar::Cigar(const char* str) : std::vector<CigarOperation>{}
{
    size_t numberStart = 0;
    size_t i = 0;
    for (const char *c = str; *c; ++i, ++c) {
        if (!std::isdigit(*c)) {
            const size_t distance = i - numberStart;
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

size_t ReferenceLength(const Cigar& cigar)
{
    size_t length = 0;
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
    results.Identity =
        100.0 * results.MatchBases / (results.MatchBases + results.MismatchBases +
                                      results.DeletionBases + results.InsertionBases);
    results.GapCompressedIdentity =
        100.0 * results.MatchBases / (results.MatchBases + results.MismatchBases +
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

}  // namespace Data
}  // namespace PacBio

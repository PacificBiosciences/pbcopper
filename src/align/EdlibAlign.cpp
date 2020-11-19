#include <pbcopper/align/EdlibAlign.h>

#include <array>

namespace PacBio {
namespace Align {

EdlibAlignment::EdlibAlignment(EdlibAlignResult aln) : Data(std::move(aln)) {}

EdlibAlignment::~EdlibAlignment() { edlibFreeAlignResult(Data); }

EdlibAlignment EdlibAlign(const std::string& query, const std::string& target,
                          const EdlibAlignConfig& config)
{
    return EdlibAlign(query.c_str(), query.size(), target.c_str(), target.size(), config);
}

EdlibAlignment EdlibAlign(const char* query, const int queryLength, const char* target,
                          const int targetLength, const EdlibAlignConfig& config)
{
    return EdlibAlignment{edlibAlign(query, queryLength, target, targetLength, config)};
}

std::vector<EdlibAlignment> EdlibAlign(const std::vector<std::string>& queries,
                                       const std::string& target, const EdlibAlignConfig& config)
{
    std::vector<EdlibAlignment> alignments;
    for (const auto& query : queries) {
        alignments.emplace_back(EdlibAlign(query, target, config));
    }
    return alignments;
}

Data::Cigar EdlibAlignmentToCigar(const unsigned char* alignment, int32_t alignmentLength)
{
    // edlib op codes: 0: '=', 1: 'I', 2: 'D', 3: 'X'
    constexpr const std::array<Data::CigarOperationType, 4> opToCigar{
        Data::CigarOperationType::SEQUENCE_MATCH, Data::CigarOperationType::INSERTION,
        Data::CigarOperationType::DELETION, Data::CigarOperationType::SEQUENCE_MISMATCH};

    Data::Cigar cigar;
    if (alignmentLength <= 0) {
        return cigar;
    }

    int32_t count = 0;
    Data::CigarOperationType prevOp = Data::CigarOperationType::UNKNOWN_OP;
    for (int32_t i = 0; i <= alignmentLength; i++) {
        if (i == alignmentLength ||
            (opToCigar[alignment[i]] != prevOp && prevOp != Data::CigarOperationType::UNKNOWN_OP)) {
            cigar.emplace_back(prevOp, count);
            count = 0;
        }
        if (i < alignmentLength) {
            prevOp = opToCigar[alignment[i]];
            count += 1;
        }
    }
    return cigar;
}

Data::Cigar EdlibAlignmentToCigar(const EdlibAlignment& alignment)
{
    return EdlibAlignmentToCigar(alignment.Data.alignment, alignment.Data.alignmentLength);
}

}  // namespace Align
}  // namespace PacBio

// Author: Armin Töpfer

#ifndef PBCOPPER_DATA_MAPPEDREAD_H
#define PBCOPPER_DATA_MAPPEDREAD_H

#include <ostream>

#include <pbcopper/data/Cigar.h>
#include <pbcopper/data/Read.h>
#include <pbcopper/data/Strand.h>

namespace PacBio {
namespace Data {

/// A MappedRead extends Read by the strand information and template anchoring
/// positions.
struct MappedRead : public Read
{
    MappedRead(const Read& read, enum Strand strand, size_t templateStart, size_t templateEnd,
               bool pinStart = false, bool pinEnd = false);
    MappedRead(const Read& read, enum Strand strand, Position templateStart, Position templateEnd,
               Cigar cigar, uint8_t mapQV);

    enum Strand Strand;
    Position TemplateStart;
    Position TemplateEnd;
    bool PinStart;
    bool PinEnd;
    class Cigar Cigar;
    uint8_t MapQuality;
};

///
/// \brief
///
/// \param read
/// \param start
/// \param end
///
void ClipToQuery(MappedRead& read, Position start, Position end);

///
/// \brief
///
/// \param read
/// \param start
/// \param end
/// \param exciseFlankingInserts
///
void ClipToReference(MappedRead& read, Position start, Position end, bool exciseFlankingInserts);

std::ostream& operator<<(std::ostream&, const MappedRead&);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_MAPPEDREAD_H

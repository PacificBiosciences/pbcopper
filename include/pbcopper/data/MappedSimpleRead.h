// Author: Derek Barnett

#ifndef PBCOPPER_DATA_MAPPEDSIMPLEREAD_H
#define PBCOPPER_DATA_MAPPEDSIMPLEREAD_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

#include <pbcopper/data/Cigar.h>
#include <pbcopper/data/SimpleRead.h>
#include <pbcopper/data/Strand.h>

namespace PacBio {
namespace Data {

class MappedSimpleRead : public SimpleRead
{
public:
    MappedSimpleRead(const SimpleRead& read, PacBio::Data::Strand strand, Position templateStart,
                     Position templateEnd, PacBio::Data::Cigar cigar, uint8_t mapQV);

    // mapping data
    PacBio::Data::Strand Strand;
    Position TemplateStart;
    Position TemplateEnd;
    PacBio::Data::Cigar Cigar;
    uint8_t MapQuality;
};

///
/// \brief
///
/// \param read
/// \param start
/// \param end
///
void ClipToQuery(MappedSimpleRead& read, Position start, Position end);

///
/// \brief
///
/// \param read
/// \param start
/// \param end
/// \param exciseFlankingInserts
///
void ClipToReference(MappedSimpleRead& read, Position start, Position end,
                     bool exciseFlankingInserts);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_SIMPLEREAD_H

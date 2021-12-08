#ifndef PBCOPPER_ALIGN_ALIGNCONFIG_H
#define PBCOPPER_ALIGN_ALIGNCONFIG_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

namespace PacBio {
namespace Align {

//
// Scoring params for Needleman-Wunsch or Smith-Waterman style aligners
//
struct AlignParams
{
    int Match;
    int Mismatch;
    int Insert;
    int Delete;

    // edit distance params
    static AlignParams Default();
};

enum struct AlignMode
{
    GLOBAL = 0,      // Global in both sequences
    SEMIGLOBAL = 1,  // Global in query, local in target
    LOCAL = 2        // Local in both sequences
};

struct AlignConfig
{
    AlignParams Params;
    AlignMode Mode;

    // edit distance params, global alignment mode
    static AlignConfig Default();
};

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_ALIGNCONFIG_H

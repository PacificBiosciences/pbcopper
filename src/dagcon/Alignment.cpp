#include <pbcopper/dagcon/Alignment.h>

#include <cassert>

#include <stdexcept>
#include <string>

namespace PacBio {
namespace Dagcon {

Alignment NormalizeGaps(const Alignment& alignment, bool push)
{
    // XXX: optimize this
    assert(alignment.Query.length() == alignment.Target.length());
    const size_t queryLength = alignment.Query.length();
    if (queryLength == 0) return alignment;

    std::string qNorm;
    qNorm.reserve(queryLength + 100);
    std::string tNorm;
    tNorm.reserve(queryLength + 100);

    // convert mismatches to indels
    for (size_t i = 0; i < queryLength; i++) {
        const char queryBase = alignment.Query[i];
        const char targetBase = alignment.Target[i];
        if (queryBase != targetBase && queryBase != '-' && targetBase != '-') {
            qNorm += '-';
            qNorm += queryBase;
            tNorm += targetBase;
            tNorm += '-';
        } else {
            qNorm += queryBase;
            tNorm += targetBase;
        }
    }

    // update length
    assert(qNorm.length() == tNorm.length());
    const size_t qNormLength = qNorm.length();

    if (push) {
        // push gaps to the right, but not past the end
        for (size_t i = 0; i < qNormLength - 1; i++) {
            // pushing target gaps
            if (tNorm[i] == '-') {
                size_t j = i;
                while (++j < qNormLength) {
                    const char c = tNorm[j];
                    if (c != '-') {
                        if (c == qNorm[i]) {
                            tNorm[i] = c;
                            tNorm[j] = '-';
                        }
                        break;
                    }
                }
            }

            // pushing query gaps
            if (qNorm[i] == '-') {
                size_t j = i;
                while (++j < qNormLength) {
                    const char c = qNorm[j];
                    if (c != '-') {
                        if (c == tNorm[i]) {
                            qNorm[i] = c;
                            qNorm[j] = '-';
                        }
                        break;
                    }
                }
            }
        }
    }
    assert(qNorm.length() == tNorm.length());
    assert(qNormLength == tNorm.length());

    // generate the final, normalized alignment strings
    Alignment finalNorm;
    finalNorm.Id = alignment.Id;
    finalNorm.SId = alignment.SId;
    finalNorm.Start = alignment.Start;
    finalNorm.TargetLength = alignment.TargetLength;
    finalNorm.Strand = alignment.Strand;
    for (size_t i = 0; i < qNormLength; i++) {
        if (qNorm[i] != '-' || tNorm[i] != '-') {
            finalNorm.Query += qNorm[i];
            finalNorm.Target += tNorm[i];
        }
    }

    return finalNorm;
}

void TrimAlignment(Alignment& alignment, int length)
{
    const auto targetLength = alignment.Target.length();

    int lbases = 0;
    size_t lOffset = 0U;
    while (lbases < length && lOffset < targetLength) {
        if (alignment.Target[lOffset] != '-') {
            ++lbases;
        }
        ++lOffset;
    }

    int rbases = 0;
    size_t rOffset = targetLength;
    while (rbases < length && rOffset > lOffset) {
        --rOffset;
        if (alignment.Target[rOffset] != '-') {
            rbases++;
        }
    }

    alignment.Start += lbases;
    alignment.Query = alignment.Query.substr(lOffset, rOffset - lOffset);
    alignment.Target = alignment.Target.substr(lOffset, rOffset - lOffset);
}

}  // namespace Dagcon
}  // namespace PacBio

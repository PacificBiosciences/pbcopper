#include <pbcopper/dagcon/Alignment.h>

#include <stdexcept>
#include <string>

#include <cassert>

namespace PacBio {
namespace Dagcon {

void NormalizeGaps(Alignment& alignment, bool push)
{
    assert(alignment.Query.length() == alignment.Target.length());
    const std::size_t queryLength = alignment.Query.length();
    if (queryLength == 0) {
        return;
    }

    std::string qNorm;
    qNorm.reserve(queryLength + 100);
    std::string tNorm;
    tNorm.reserve(queryLength + 100);

    // convert mismatches to indels
    for (std::size_t i = 0; i < queryLength; i++) {
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
    const std::size_t qNormLength = qNorm.length();

    if (push) {
        // push gaps to the right, but not past the end
        for (std::size_t i = 0; i < qNormLength - 1; i++) {
            // pushing target gaps
            if (tNorm[i] == '-') {
                std::size_t j = i;
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
                std::size_t j = i;
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

    // update alignment sequence
    alignment.Query.clear();
    alignment.Target.clear();
    for (std::size_t i = 0; i < qNormLength; ++i) {
        if (qNorm[i] != '-' || tNorm[i] != '-') {
            alignment.Query += qNorm[i];
            alignment.Target += tNorm[i];
        }
    }
}

void TrimAlignment(Alignment& alignment, int trimLength)
{
    const auto targetLength = alignment.Target.length();

    int lbases = 0;
    std::size_t lOffset = 0U;
    while (lbases < trimLength && lOffset < targetLength) {
        if (alignment.Target[lOffset] != '-') {
            ++lbases;
        }
        ++lOffset;
    }

    int rbases = 0;
    std::size_t rOffset = targetLength;
    while (rbases < trimLength && rOffset > lOffset) {
        --rOffset;
        if (alignment.Target[rOffset] != '-') {
            ++rbases;
        }
    }

    alignment.Start += lbases;
    alignment.Query = alignment.Query.substr(lOffset, rOffset - lOffset);
    alignment.Target = alignment.Target.substr(lOffset, rOffset - lOffset);
}

}  // namespace Dagcon
}  // namespace PacBio

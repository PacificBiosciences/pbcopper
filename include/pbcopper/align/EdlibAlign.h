#ifndef PBCOPPER_ALIGN_EDLIBALIGN_H
#define PBCOPPER_ALIGN_EDLIBALIGN_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Cigar.h>
#include <pbcopper/third-party/edlib.h>

#include <memory>
#include <string>
#include <vector>

namespace PacBio {
namespace Align {

///
/// RAII wrapper around edlib's alignment result
///
struct EdlibAlignment
{
    explicit EdlibAlignment(EdlibAlignResult aln);
    EdlibAlignment(const EdlibAlignment&) = delete;
    EdlibAlignment(EdlibAlignment&&) noexcept = default;
    EdlibAlignment& operator=(const EdlibAlignment&) = delete;
    EdlibAlignment& operator=(EdlibAlignment&&) noexcept = default;
    ~EdlibAlignment() noexcept;

    EdlibAlignResult Data;
};

///
/// Align query to target
///
EdlibAlignment EdlibAlign(const std::string& query, const std::string& target,
                          const EdlibAlignConfig& config);
EdlibAlignment EdlibAlign(const char* query, int queryLength, const char* target, int targetLength,
                          const EdlibAlignConfig& config);

///
/// Align queries to target
///
std::vector<std::unique_ptr<EdlibAlignment>> EdlibAlign(const std::vector<std::string>& queries,
                                                        const std::string& target,
                                                        const EdlibAlignConfig& config);

///
/// Convert edlib alignment result to CIGAR
///
Data::Cigar EdlibAlignmentToCigar(const EdlibAlignment& alignment);
Data::Cigar EdlibAlignmentToCigar(const unsigned char* alignment, int32_t alignmentLength);

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_EDLIBALIGN_H

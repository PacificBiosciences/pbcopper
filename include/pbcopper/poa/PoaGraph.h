#ifndef PBCOPPER_POA_POAGRAPH_H
#define PBCOPPER_POA_POAGRAPH_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/AlignConfig.h>

#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <climits>
#include <cstddef>

namespace PacBio {
namespace Poa {

// fwd decls
namespace detail {

class PoaGraphImpl;
class SdpRangeFinder;

}  // namespace detail

struct PoaConsensus;

class PoaAlignmentMatrix
{
public:
    virtual ~PoaAlignmentMatrix() = default;
    virtual float Score() const = 0;
    virtual std::size_t NumRows() const = 0;
    virtual std::size_t NumCols() const = 0;
    virtual void Print() const = 0;
};

/// \brief An object representing a Poa (partial-order alignment) graph
class PoaGraph
{
public:
    using Vertex = std::size_t;
    using ReadId = std::size_t;

    static const Vertex NullVertex = std::numeric_limits<Vertex>::max();

public:  // Flags enums for specifying GraphViz output features
    enum
    {
        COLOR_NODES = 0x1,
        VERBOSE_NODES = 0x2,
    };

public:
    PoaGraph();

    PoaGraph(const PoaGraph& other);
    PoaGraph& operator=(const PoaGraph& other);

    PoaGraph(PoaGraph&&) noexcept;
    PoaGraph& operator=(PoaGraph&&) noexcept;

    PoaGraph(const detail::PoaGraphImpl& o);  // NB: this performs a copy

    ~PoaGraph();

    //
    // Easy API
    //
    void AddRead(const std::string& sequence, const Align::AlignConfig& config,
                 detail::SdpRangeFinder* rangeFinder = NULL,
                 std::vector<Vertex>* readPathOutput = NULL);

    //
    // API for more control
    //
    void AddFirstRead(const std::string& sequence, std::vector<Vertex>* readPathOutput = NULL);

    PoaAlignmentMatrix* TryAddRead(const std::string& sequence, const Align::AlignConfig& config,
                                   detail::SdpRangeFinder* const rangeFinder = NULL) const;

    void CommitAdd(PoaAlignmentMatrix* mat, std::vector<Vertex>* readPathOutput = NULL);

    void PruneGraph(int minCoverage);

    // ----------

    std::size_t NumReads() const;

    std::string ToGraphViz(int flags = 0, const PoaConsensus* pc = NULL) const;

    void WriteGraphVizFile(const std::filesystem::path& filename, int flags = 0,
                           const PoaConsensus* pc = NULL) const;

    void WriteGraphCsvFile(const std::filesystem::path& filename) const;

    const PoaConsensus* FindConsensus(const Align::AlignConfig& config,
                                      int minCoverage = -INT_MAX) const;

private:
    std::unique_ptr<detail::PoaGraphImpl> impl_;
};

}  // namespace Poa
}  // namespace PacBio

#endif  // PBCOPPER_POA_POAGRAPH_H

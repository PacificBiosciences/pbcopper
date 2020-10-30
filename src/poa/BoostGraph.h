#ifndef PBCOPPER_POA_BOOSTGRAPH_H
#define PBCOPPER_POA_BOOSTGRAPH_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace boost {
enum vertex_info_t
{
    vertex_info = 424
};  // a unique #
BOOST_INSTALL_PROPERTY(vertex, info);
}  // namespace boost

namespace PacBio {
namespace Poa {
namespace detail {

class PoaNode;

// BGL is intimidating, and it *deserves* your hatred.  But it's
// the only game in town!
using vertex_property_t =
    boost::property<boost::vertex_info_t, PoaNode, boost::property<boost::vertex_index_t, size_t> >;
using BoostGraph =
    boost::adjacency_list<boost::setS, boost::listS, boost::bidirectionalS, vertex_property_t>;

// Descriptor types used internally
using ED = boost::graph_traits<BoostGraph>::edge_descriptor;
using VD = boost::graph_traits<BoostGraph>::vertex_descriptor;

using VertexInfoMap = boost::property_map<BoostGraph, boost::vertex_info_t>::type;
using index_map_t = boost::property_map<BoostGraph, boost::vertex_index_t>::type;
static const VD null_vertex = boost::graph_traits<BoostGraph>::null_vertex();

}  // namespace detail
}  // namespace Poa
}  // namespace PacBio
#endif  // PBCOPPER_POA_BOOSTGRAPH_H

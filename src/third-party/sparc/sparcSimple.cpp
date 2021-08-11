#include <pbcopper/third-party/sparc/sparc.hpp>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/container/small_vector.hpp>

namespace Sparc {
namespace SparcSimpleImpl {

struct EdgeNode;
struct Node;

using PtrEdgeNode = EdgeNode*;
using PtrNode = Node*;

struct EdgeNode
{
    int32_t EdgeCov = 0;
    PtrNode NodePtr = nullptr;
    PtrEdgeNode NextEdge = nullptr;
};

struct Node
{
    char Base = 0;
    int32_t Score = 0;
    int32_t Cov = 0;
    bool InBackbone = false;
    PtrEdgeNode Left = nullptr;
    PtrEdgeNode Right = nullptr;
    PtrNode LastNode = nullptr;
};

struct Backbone
{
    std::vector<PtrNode> Nodes;
    std::vector<int32_t> Covs;
};

template <typename T>
struct Bucket
{
    ~Bucket()
    {
        for (auto& item : Data) {
            if (item) {
                delete item;
            }
        }
    }

    void Add(T item) { Data.emplace_back(item); }

    std::vector<T> Data;
};

using EdgeBucket = Bucket<PtrEdgeNode>;
using NodeBucket = Bucket<PtrNode>;

void ConsensusKmerGraphConstruction(const std::string& ref, Backbone& backbone,
                                    EdgeBucket& edgeBucket, NodeBucket& nodeBucket)
{
    const int32_t refLen = ref.size();
    PtrNode previous_node = NULL;
    PtrNode current_node = NULL;
    backbone.Nodes.resize(refLen);

    for (int32_t j = 0; j < refLen; ++j) {
        previous_node = current_node;
        current_node = new Node();
        nodeBucket.Add(current_node);
        backbone.Nodes[j] = current_node;
        current_node->Base = ref[j];
        current_node->InBackbone = true;
        ++current_node->Cov;
        if (j >= 1) {
            previous_node->Right = new EdgeNode();
            edgeBucket.Add(previous_node->Right);
            previous_node->Right->NodePtr = current_node;

            current_node->Left = new EdgeNode();
            edgeBucket.Add(current_node->Left);
            current_node->Left->NodePtr = previous_node;
        }
    }
}

void NormalizeAlignment(Query& query)
{
    std::string qAlignedSeqNew;
    std::string tAlignedSeqNew;
    const int32_t refLen = query.tAlignedSeq.size();
    const int32_t qryLen = query.qAlignedSeq.size();
    qAlignedSeqNew.resize(2 * qryLen);
    tAlignedSeqNew.resize(2 * qryLen);
    int32_t curPos = 0;
    for (int32_t i = 0; i < qryLen; ++i) {
        char& curQry = query.qAlignedSeq[i];
        char& curRef = query.tAlignedSeq[i];
        if (curQry == curRef) {
            qAlignedSeqNew[curPos] = curQry;
            tAlignedSeqNew[curPos] = curRef;
            ++curPos;
        } else {
            if (curQry != '-' && curRef != '-') {
                tAlignedSeqNew[curPos] = curRef;
                ++curPos;
                bool replace = false;
                for (int32_t j = i + 1; j < refLen; ++j) {
                    auto& curJRef = query.tAlignedSeq[j];
                    if (curJRef != '-') {
                        if (curJRef == curQry) {
                            replace = true;
                            curJRef = '-';
                            tAlignedSeqNew[curPos] = curQry;
                        }
                        break;
                    }
                }
                if (!replace) {
                    tAlignedSeqNew[curPos] = '-';
                }
                --curPos;

                qAlignedSeqNew[curPos] = '-';
                ++curPos;
                qAlignedSeqNew[curPos] = curQry;
                ++curPos;
            } else {
                if (curQry == '-') {
                    for (int32_t j = i + 1; j < qryLen; ++j) {
                        auto& curJQry = query.qAlignedSeq[j];
                        if (curJQry != '-') {
                            if (curJQry == curRef) {
                                curQry = curJQry;
                                curJQry = '-';
                            }
                            break;
                        }
                    }
                } else {
                    for (int32_t j = i + 1; j < qryLen; ++j) {
                        auto& curJRef = query.tAlignedSeq[j];
                        if (curJRef != '-') {
                            if (curJRef == curQry) {
                                curRef = curJRef;
                                curJRef = '-';
                            }
                            break;
                        }
                    }
                }

                qAlignedSeqNew[curPos] = curQry;
                tAlignedSeqNew[curPos] = curRef;
                ++curPos;
            }
        }
    }
    qAlignedSeqNew.resize(curPos);
    tAlignedSeqNew.resize(curPos);
    query.qAlignedSeq = std::move(qAlignedSeqNew);
    query.tAlignedSeq = std::move(tAlignedSeqNew);
}

void AddPathToBackbone(Backbone& backbone, Query& query, EdgeBucket& edgeBucket,
                       NodeBucket& nodeBucket)
{
    NormalizeAlignment(query);

    int32_t target_position = query.tStart;
    Node* previous_node = NULL;
    Node* current_node = NULL;
    int32_t MatchPosition = -100;
    int32_t backboneLength = backbone.Nodes.size();

    const int32_t qryLen = query.qAlignedSeq.size();
    for (int32_t i = 0; i < qryLen; ++i) {
        if (query.qAlignedSeq[i] == '-') {
            MatchPosition = -1;
            if (query.tAlignedSeq[i] != '-') {
                ++target_position;
            }
            continue;
        }
        MatchPosition = target_position;
        if (query.qAlignedSeq[i] != query.tAlignedSeq[i]) {
            MatchPosition = -1;
        }

        char base{' '};
        for (int32_t j = i; j < qryLen; ++j) {
            if (query.qAlignedSeq[j] != '-') {
                base = query.qAlignedSeq[j];
                break;
            }
        }

        if (base == ' ') {
            break;
        }

        if (MatchPosition >= backboneLength) {
            break;
        }

        if (MatchPosition >= 0) {
            current_node = backbone.Nodes[MatchPosition];

            if (previous_node != NULL) {
                ++current_node->Cov;
            }

            // link previous node to the current one
            if (previous_node != NULL) {
                PtrEdgeNode* edge_p2p = &(previous_node->Right);

                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr == current_node) {
                        ++(*edge_p2p)->EdgeCov;
                        break;
                    }
                    edge_p2p = &((*edge_p2p)->NextEdge);
                }

                if (*edge_p2p == NULL) {
                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    (*edge_p2p)->NodePtr = current_node;
                    ++(*edge_p2p)->EdgeCov;
                }

                edge_p2p = &(current_node->Left);
                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr == previous_node) {
                        ++(*edge_p2p)->EdgeCov;
                        break;
                    }
                    edge_p2p = &((*edge_p2p)->NextEdge);
                }
                if (*edge_p2p == NULL) {
                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    (*edge_p2p)->NodePtr = previous_node;
                    ++(*edge_p2p)->EdgeCov;
                }
            }

        } else {

            //link previous node to the current one

            if (previous_node == NULL) {
                current_node = new Node();
                nodeBucket.Add(current_node);
                current_node->Base = base;
                ++current_node->Cov;
                previous_node = current_node;
            } else {
                PtrEdgeNode* edge_p2p = &(previous_node->Right);
                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr->Base == base && !(*edge_p2p)->NodePtr->InBackbone) {
                        ++(*edge_p2p)->EdgeCov;
                        current_node = (*edge_p2p)->NodePtr;
                        ++current_node->Cov;
                        break;
                    }
                    edge_p2p = &((*edge_p2p)->NextEdge);
                }

                if (*edge_p2p == NULL) {
                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    current_node = new Node();
                    nodeBucket.Add(current_node);
                    current_node->Base = base;
                    ++current_node->Cov;
                    (*edge_p2p)->NodePtr = current_node;
                    ++(*edge_p2p)->EdgeCov;
                }

                // link current node to previous
                edge_p2p = &(current_node->Left);
                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr->Base == previous_node->Base) {
                        ++(*edge_p2p)->EdgeCov;
                        break;
                    }
                    edge_p2p = &((*edge_p2p)->NextEdge);
                }

                if (*edge_p2p == NULL) {
                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    (*edge_p2p)->NodePtr = previous_node;
                    ++(*edge_p2p)->EdgeCov;
                }
            }
        }

        previous_node = current_node;

        if (query.tAlignedSeq[i] != '-') {
            ++target_position;
        }
    }
}

void BFSFindBestPath(Backbone& backbone, int32_t node_idx, int32_t minCov, double threshold)
{
    PtrNode begin_node = backbone.Nodes[node_idx];
    std::list<PtrNode> node_list;
    node_list.push_back(begin_node);
    int32_t max_cov = backbone.Covs[node_idx];
    while (node_list.size() > 0) {
        PtrNode current_node = node_list.front();
        node_list.pop_front();
        PtrEdgeNode edge_ptr = current_node->Right;
        while (edge_ptr != NULL) {
            bool update = 0;
            int32_t newScore = 0;
            int32_t newThreshold =
                std::max(static_cast<int32_t>(std::round((max_cov * threshold))), minCov);
            newScore = (current_node->Score + edge_ptr->EdgeCov - newThreshold);
            if (edge_ptr->NodePtr->Score < newScore) {
                update = 1;
            }

            if (update) {
                edge_ptr->NodePtr->Cov = current_node->Cov + 1;
                edge_ptr->NodePtr->Score = newScore;
                edge_ptr->NodePtr->LastNode = current_node;
                if (!edge_ptr->NodePtr->InBackbone) {
                    node_list.push_back(edge_ptr->NodePtr);
                }
            }

            edge_ptr = edge_ptr->NextEdge;
        }
    }
}

void FindBestPath(Backbone& backbone, int32_t minCov, double threshold)
{
    backbone.Nodes[0]->Cov = 1;  //depth
    const int32_t numNodex = backbone.Nodes.size();
    for (int32_t i = 0; i + 1 < numNodex; ++i) {
        BFSFindBestPath(backbone, i, minCov, threshold);
    }
}

}  // namespace SparcSimpleImpl
std::string SparcSimpleConsensus(const std::string& backboneSeq, std::vector<Query>& queries,
                                 const int32_t minCov, const double threshold)
{
    SparcSimpleImpl::Backbone backbone;

    SparcSimpleImpl::EdgeBucket edgeBucket;
    SparcSimpleImpl::NodeBucket nodeBucket;

    SparcSimpleImpl::ConsensusKmerGraphConstruction(backboneSeq, backbone, edgeBucket, nodeBucket);

    for (Query& q : queries) {
        SparcSimpleImpl::AddPathToBackbone(backbone, q, edgeBucket, nodeBucket);
    }

    const int32_t backboneSeqLen = backboneSeq.size();
    // hopefully never goes zero
    size_t radius = std::min(std::min<size_t>(200, backbone.Nodes.size()),
                             static_cast<size_t>(backboneSeqLen - 1));
    std::map<int, int> cov_cnt;
    backbone.Covs.resize(backbone.Nodes.size());

    for (size_t i = 0; i < radius; ++i) {
        if (backbone.Nodes[i] != NULL) {
            ++cov_cnt[backbone.Nodes[i]->Cov];
        }
    }

    for (size_t i = 0; i < backbone.Nodes.size(); ++i) {
        if (i >= radius) {
            if (backbone.Nodes[i - radius] != NULL) {
                --cov_cnt[backbone.Nodes[i - radius]->Cov];
                if (cov_cnt[backbone.Nodes[i - radius]->Cov] <= 0) {
                    cov_cnt.erase(backbone.Nodes[i - radius]->Cov);
                }
            }
        }
        if ((i + radius < backbone.Nodes.size()) && (backbone.Nodes[i + radius] != NULL)) {
            ++cov_cnt[backbone.Nodes[i + radius]->Cov];
        }

        if (cov_cnt.size() > 0) {
            backbone.Covs[i] = cov_cnt.rbegin()->first;
        } else {
            backbone.Covs[i] = 0;
        }
    }

    SparcSimpleImpl::FindBestPath(backbone, minCov, threshold);

    std::string consensus;
    int64_t maxScore = 0;
    size_t position = 0;

    for (size_t i = 0; i < backbone.Nodes.size(); ++i) {
        if (backbone.Nodes[i]->Score > maxScore) {
            maxScore = backbone.Nodes[i]->Score;
            position = i;
        }
    }

    if (maxScore == 0) {
        return "";
    }

    SparcSimpleImpl::PtrNode current_node = backbone.Nodes[position];

    if (current_node != NULL) {
        char base = current_node->Base;
        consensus = base;
        current_node = current_node->LastNode;
        while (current_node != NULL) {
            consensus.push_back(current_node->Base);
            current_node = current_node->LastNode;
        }

        std::reverse(consensus.begin(), consensus.end());
    }

    int32_t cns_pos = consensus.size();
    current_node = backbone.Nodes[position];
    backbone.Nodes.clear();
    backbone.Nodes.push_back(current_node);
    if (current_node != NULL) {
        current_node = current_node->LastNode;
        while (current_node != NULL) {
            backbone.Nodes.push_back(current_node);
            --cns_pos;
            current_node = current_node->LastNode;
        }
    }
    std::reverse(backbone.Nodes.begin(), backbone.Nodes.end());

    return consensus;
}
}  // namespace Sparc

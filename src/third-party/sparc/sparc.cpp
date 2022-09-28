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
namespace {

struct Read
{
    std::vector<uint64_t> ReadBits;
    size_t ReadLen;
};

struct EdgeNode;
struct Node;

using PtrEdgeNode = EdgeNode*;
using PtrNode = Node*;

struct EdgeNode
{
    uint32_t Edge = 0;
    int32_t EdgeCov = 0;
    size_t Len = 0;
    PtrNode NodePtr = nullptr;
    PtrEdgeNode NextEdge = nullptr;
};

struct Node
{
    uint32_t Kmer = 0;
    int64_t Score = 0;
    uint32_t Cov = 0;
    bool InBackbone = false;
    PtrEdgeNode Left = nullptr;
    PtrEdgeNode Right = nullptr;
    PtrNode LastNode = nullptr;
};

struct Backbone
{
    std::vector<PtrNode> Nodes;
    std::vector<uint64_t> Covs;
    int32_t MinCov = 0;
    int32_t Gap = 0;
    double Threshold = 0.0;
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

//left shift and right shift of shift_sz bits of the whole bit array, arr_sz is the array length
static inline void LShiftNB(uint64_t* bitsarr, int shift_sz, int arr_sz)
{
    boost::container::small_vector<uint64_t, 10> temp_arr(arr_sz);
    int jmp = shift_sz / 64;
    int offset = shift_sz % 64;

    for (int i = 0; i < arr_sz; ++i) {
        if (i + jmp + 1 < arr_sz) {

            uint64_t tt = 0;
            if (offset == 0) {
                tt = 0;
            } else {
                tt = (bitsarr[i + jmp + 1] >> (64 - offset));
            }
            temp_arr[i] = ((bitsarr[i + jmp] << offset) | tt);
        }
        if (i + jmp + 1 == arr_sz) {
            temp_arr[i] = bitsarr[i + jmp] << offset;
        }
        if (i + jmp + 1 > arr_sz) {
            temp_arr[i] = 0;
        }
    }

    std::memcpy(bitsarr, temp_arr.data(), sizeof(uint64_t) * arr_sz);
}

static inline void RShiftNB(uint64_t* bitsarr, int shift_sz, int arr_sz)
{
    boost::container::small_vector<uint64_t, 10> temp_arr(arr_sz);
    int jmp = shift_sz / 64;
    int offset = shift_sz % 64;

    for (int i = arr_sz - 1; i >= 0; --i) {
        if (i - jmp > 0) {
            if (offset > 0) {
                temp_arr[i] =
                    (bitsarr[i - jmp] >> offset) | (bitsarr[i - jmp - 1] << (64 - offset));
            } else {
                temp_arr[i] = bitsarr[i - jmp];
            }
        }
        if (i - jmp == 0) {
            if (offset > 0) {
                temp_arr[i] = (bitsarr[i - jmp] >> offset);
            } else {
                temp_arr[i] = bitsarr[i - jmp];
            }
        }
        if (i - jmp < 0) {
            temp_arr[i] = 0;
        }
    }
    std::memcpy(bitsarr, temp_arr.data(), sizeof(uint64_t) * arr_sz);
}

// get sub bit array from a bit array.
inline void GetSubArray(uint64_t* bitsarr_in, int bitsarr_len, int begin_pos, int sub_sz,
                        uint64_t* bitsarr_out)
{
    if (bitsarr_len < sub_sz) {
        throw std::runtime_error("Input Kmer too short.");
    }
    int rem = bitsarr_len % 32;

    int arr_sz_out = sub_sz / 32 + 1;
    if (sub_sz % 32 == 0) {
        arr_sz_out--;
    }

    boost::container::small_vector<uint64_t, 10> temp_arr(10);

    std::memset(bitsarr_out, 0, sizeof(uint64_t) * arr_sz_out);

    int rem2 = (32 - rem + begin_pos) % 32;
    int block_beg = (32 - rem + begin_pos) / 32;
    if (rem == 0) {
        block_beg--;
    }

    int rem3 = (32 - rem + begin_pos + sub_sz) % 32;
    int block_end = (32 - rem + begin_pos + sub_sz) / 32;
    if (rem3 != 0) {
        rem3 = 32 - rem3;
    } else {
        block_end--;
    }
    if (rem == 0) {
        block_end--;
    }

    int orig_sz = (block_end - block_beg + 1);
    if (orig_sz > 10) {
        throw std::runtime_error("GetSubArray, First: " + std::to_string(orig_sz));
    }
    std::memcpy(&temp_arr[0], &bitsarr_in[block_beg], orig_sz * sizeof(uint64_t));
    LShiftNB(&temp_arr[0], rem2 * 2, orig_sz);
    RShiftNB(&temp_arr[0], (rem2 + rem3) % 32 * 2, arr_sz_out);
    if (arr_sz_out > 10) {
        throw std::runtime_error("GetSubArray, Second: " + std::to_string(orig_sz));
    }
    std::memcpy(bitsarr_out, temp_arr.data(), arr_sz_out * sizeof(uint64_t));
}

uint64_t* Str2BitArray(const std::string& str, size_t len, uint64_t* b_str, size_t arr_sz)
{
    for (size_t k = 0; k < arr_sz; ++k) {
        b_str[k] = 0;
    }
    int arr_sz_needed = len / 32 + 1;
    int rem = len % 32;
    if (rem == 0) {
        arr_sz_needed--;
    }

    int beg_arr_idx = arr_sz - arr_sz_needed;
    if (rem == 0 && arr_sz_needed > 0) {
        rem = 32;
    }
    for (size_t k = 0; k < len; k++) {
        if (rem == 0) {
            beg_arr_idx++;
            rem = 32;
        }

        switch (str[k]) {
            case ('A'):
            case ('a'):
            case ('0'):
                b_str[beg_arr_idx] <<= 2;  //LShiftNB(b_str,2,arr_sz);
                rem--;
                break;

            case ('C'):
            case ('c'):
            case ('1'):
                b_str[beg_arr_idx] <<= 2;  //LShiftNB(b_str,2,arr_sz);
                ++b_str[beg_arr_idx];      //++b_str[arr_sz-1];
                rem--;
                break;

            case 'G':
            case 'g':
            case '2':
                b_str[beg_arr_idx] <<= 1;  //LShiftNB(b_str,1,arr_sz);
                ++b_str[beg_arr_idx];      //++b_str[arr_sz-1];
                b_str[beg_arr_idx] <<= 1;  //LShiftNB(b_str,1,arr_sz);
                rem--;                     //(++(b_str<<=1))<<=1;
                break;

            case 'T':
            case 't':
            case '3':
                b_str[beg_arr_idx] <<= 1;  //LShiftNB(b_str,1,arr_sz);
                ++b_str[beg_arr_idx];
                b_str[beg_arr_idx] <<= 1;  //LShiftNB(b_str,1,arr_sz);
                ++b_str[beg_arr_idx];
                rem--;
                break;
            default:
                return b_str;
        }
    }
    return b_str;
}

//convert a std::string of nucleotide bases into bit array.
void InitRefRead(const std::string& seq, Read& read)
{
    read.ReadLen = strlen(seq.c_str());
    size_t Read_arr_sz = read.ReadLen / 32 + 1;
    int rem = read.ReadLen % 32;
    if (rem == 0) {
        Read_arr_sz--;
    }

    Str2BitArray(seq, seq.size(), read.ReadBits.data(), Read_arr_sz);
}

char* BitsArray2Str(uint64_t* b_seq, int len, char* c_str)
{
    constexpr int32_t maxL = 32;
    int tot_bits = 64;
    assert(len < maxL);
    if (len >= maxL) throw std::runtime_error("BitsArray2Str");
    for (int i = 0; i < len; ++i) {
        uint64_t temp;
        boost::container::small_vector<uint64_t, maxL> temp2(maxL);
        temp2[0] = b_seq[0];
        LShiftNB(temp2.data(), tot_bits - (len - i) * 2, 1);
        RShiftNB(temp2.data(), tot_bits - 2, 1);
        temp = temp2[0];
        switch (temp) {
            case 0:
                c_str[i] = 'A';
                break;
            case 1:
                c_str[i] = 'C';
                break;
            case 2:
                c_str[i] = 'G';
                break;
            case 3:
                c_str[i] = 'T';
                break;
        }
    }
    c_str[len] = '\0';
    return c_str;
}

void ConsensusKmerGraphConstruction(struct Read* read, Backbone* backbone, int k,
                                    EdgeBucket& edgeBucket, NodeBucket& nodeBucket)
{
    const int readLen = read->ReadLen;
    int overlappingKmers = readLen - k + 1;

    uint64_t seq;

    PtrNode previous_node = NULL;
    PtrNode current_node = NULL;
    backbone->Nodes.resize(overlappingKmers);

    for (int j = 0; j < overlappingKmers; j++) {

        previous_node = current_node;
        GetSubArray(read->ReadBits.data(), read->ReadLen, j, k, &seq);

        current_node = new Node();
        nodeBucket.Add(current_node);
        backbone->Nodes[j] = current_node;
        current_node->Kmer = static_cast<uint32_t>(seq);
        current_node->InBackbone = true;
        current_node->Cov++;
        if (j >= 1) {
            //left edge,right edge
            previous_node->Right = new EdgeNode();
            edgeBucket.Add(previous_node->Right);
            previous_node->Right->NodePtr = current_node;

            current_node->Left = new EdgeNode();
            edgeBucket.Add(current_node->Left);
            current_node->Left->NodePtr = previous_node;
        }
    }
}

void ConsensusSparseKmerGraphConstruction(struct Read* read, Backbone* backbone, int k,
                                          EdgeBucket& edgeBucket, NodeBucket& nodeBucket)
{
    const int gap = backbone->Gap;

    const int readLen = read->ReadLen;
    const int overlappingKmers = readLen - k + 1;

    uint64_t seq;

    PtrNode previous_node = NULL;
    PtrNode current_node = NULL;
    backbone->Nodes.resize(overlappingKmers);

    int last_position = -1;
    for (int j = 0; j < overlappingKmers; j++) {
        if (j % gap == 0) {

            previous_node = current_node;
            GetSubArray(read->ReadBits.data(), read->ReadLen, j, k, &seq);

            current_node = new Node();
            nodeBucket.Add(current_node);
            backbone->Nodes[j] = current_node;
            current_node->Kmer = static_cast<uint32_t>(seq);
            current_node->InBackbone = true;
            current_node->Cov++;

            //left edge,right edge
            uint64_t edge_bits;
            if (j >= 1) {
                GetSubArray(read->ReadBits.data(), read->ReadLen, last_position + k, gap,
                            &edge_bits);
                previous_node->Right = new EdgeNode();
                edgeBucket.Add(previous_node->Right);
                previous_node->Right->NodePtr = current_node;
                previous_node->Right->Edge = edge_bits;
                previous_node->Right->Len = gap;

                GetSubArray(read->ReadBits.data(), read->ReadLen, last_position, gap, &edge_bits);
                current_node->Left = new EdgeNode();
                edgeBucket.Add(current_node->Left);
                current_node->Left->NodePtr = previous_node;
                current_node->Left->Len = gap;
                current_node->Left->Edge = edge_bits;
            }
            last_position = j;
        }
    }
}

void NormalizeAlignment(Query* query)
{

    std::string qAlignedSeq_new, tAlignedSeq_new;
    size_t seq_sz = query->qAlignedSeq.size();
    qAlignedSeq_new.resize(2 * seq_sz);
    tAlignedSeq_new.resize(2 * seq_sz);
    std::string target_crop, query_crop;
    int n_char = 0;
    for (size_t i = 0; i < seq_sz; ++i) {
        if (query->qAlignedSeq[i] == query->tAlignedSeq[i]) {
            qAlignedSeq_new[n_char] = query->qAlignedSeq[i];
            tAlignedSeq_new[n_char] = query->tAlignedSeq[i];
            n_char++;
        } else {
            if (query->qAlignedSeq[i] != '-' && query->tAlignedSeq[i] != '-') {
                tAlignedSeq_new[n_char] = query->tAlignedSeq[i];
                n_char++;

                bool replace = 0;
                for (size_t j = i + 1; j < query->tAlignedSeq.size(); ++j) {
                    if (query->tAlignedSeq[j] != '-') {
                        if (query->tAlignedSeq[j] == query->qAlignedSeq[i]) {
                            replace = 1;
                            query->tAlignedSeq[j] = '-';
                            tAlignedSeq_new[n_char] = query->qAlignedSeq[i];
                        }
                        break;
                    }
                }
                if (replace == 0) {
                    tAlignedSeq_new[n_char] = '-';
                }
                n_char--;

                qAlignedSeq_new[n_char] = '-';
                n_char++;
                qAlignedSeq_new[n_char] = query->qAlignedSeq[i];

                n_char++;

            } else {
                if (query->qAlignedSeq[i] == '-') {

                    for (size_t j = i + 1; j < query->qAlignedSeq.size(); ++j) {
                        if (query->qAlignedSeq[j] != '-') {
                            if (query->qAlignedSeq[j] == query->tAlignedSeq[i]) {
                                query->qAlignedSeq[i] = query->qAlignedSeq[j];
                                query->qAlignedSeq[j] = '-';
                            }
                            break;
                        }
                    }

                } else {

                    for (size_t j = i + 1; j < query->qAlignedSeq.size(); ++j) {
                        if (query->tAlignedSeq[j] != '-') {
                            if (query->tAlignedSeq[j] == query->qAlignedSeq[i]) {
                                query->tAlignedSeq[i] = query->tAlignedSeq[j];
                                query->tAlignedSeq[j] = '-';
                            }
                            break;
                        }
                    }
                }

                qAlignedSeq_new[n_char] = query->qAlignedSeq[i];
                tAlignedSeq_new[n_char] = query->tAlignedSeq[i];
                n_char++;
            }
        }
    }
    qAlignedSeq_new.resize(n_char);
    tAlignedSeq_new.resize(n_char);
    query->qAlignedSeq = qAlignedSeq_new;
    query->tAlignedSeq = tAlignedSeq_new;
}

void AddPathToBackbone(Backbone* backbone, Query* query, int k, EdgeBucket& edgeBucket,
                       NodeBucket& nodeBucket)
{
    NormalizeAlignment(query);

    int target_position = query->tStart;
    Node* previous_node = NULL;
    Node* current_node = NULL;
    int MatchPosition = -100;
    int32_t backboneLength = backbone->Nodes.size();

    for (size_t i = 0; i + k <= query->qAlignedSeq.size(); ++i) {
        if (query->qAlignedSeq[i] == '-') {
            MatchPosition = -1;
            if (query->tAlignedSeq[i] != '-') {
                target_position++;
            }
            continue;
        }
        MatchPosition = target_position;
        for (int j = 0; j < k; ++j) {
            if (query->qAlignedSeq[i + j] != query->tAlignedSeq[i + j]) {
                MatchPosition = -1;
                break;
            }
        }

        std::string KmerStr;
        KmerStr.resize(k);
        int KmerSize = 0;
        for (size_t j = i; j < query->qAlignedSeq.size(); ++j) {
            if (query->qAlignedSeq[j] != '-') {
                KmerStr[KmerSize] = query->qAlignedSeq[j];
                KmerSize++;
            }
            if (KmerSize == k) {
                break;
            }
        }
        if (KmerSize != k) {
            break;
        }
        uint64_t seq = 0;
        Str2BitArray(KmerStr, k, &seq, 1);

        if (MatchPosition >= backboneLength) {
            break;
        }
        if (MatchPosition >= 0) {
            current_node = backbone->Nodes[MatchPosition];

            if (previous_node != NULL) {
                current_node->Cov++;
            }

            //link previous node to the current one

            if (previous_node != NULL) {

                PtrEdgeNode* edge_p2p = &(previous_node->Right);

                while (*edge_p2p != NULL) {

                    if ((*edge_p2p)->NodePtr == current_node) {
                        (*edge_p2p)->EdgeCov++;

                        break;
                    }

                    edge_p2p = &((*edge_p2p)->NextEdge);
                }

                if (*edge_p2p == NULL) {

                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    (*edge_p2p)->NodePtr = current_node;
                    (*edge_p2p)->EdgeCov++;
                }

                edge_p2p = &(current_node->Left);
                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr == previous_node) {
                        (*edge_p2p)->EdgeCov++;

                        break;
                    }
                    edge_p2p = &((*edge_p2p)->NextEdge);
                }
                if (*edge_p2p == NULL) {
                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    (*edge_p2p)->NodePtr = previous_node;
                    (*edge_p2p)->EdgeCov++;
                }
            }

        } else {

            //link previous node to the current one

            if (previous_node == NULL) {
                current_node = new Node();
                nodeBucket.Add(current_node);
                current_node->Kmer = static_cast<uint32_t>(seq);
                current_node->Cov++;
                previous_node = current_node;
            } else {

                PtrEdgeNode* edge_p2p = &(previous_node->Right);
                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr->Kmer == static_cast<uint32_t>(seq) &&
                        (*edge_p2p)->NodePtr->InBackbone == 0) {
                        (*edge_p2p)->EdgeCov++;
                        current_node = (*edge_p2p)->NodePtr;
                        current_node->Cov++;

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
                    current_node->Kmer = static_cast<uint32_t>(seq);
                    current_node->Cov++;
                    (*edge_p2p)->NodePtr = current_node;

                    (*edge_p2p)->EdgeCov++;
                }
                //link current node to previous
                edge_p2p = &(current_node->Left);
                while (*edge_p2p != NULL) {
                    if ((*edge_p2p)->NodePtr->Kmer == previous_node->Kmer) {
                        (*edge_p2p)->EdgeCov++;
                        break;
                    }
                    edge_p2p = &((*edge_p2p)->NextEdge);
                }
                if (*edge_p2p == NULL) {
                    (*edge_p2p) = new EdgeNode();
                    edgeBucket.Add(*edge_p2p);
                    (*edge_p2p)->NextEdge = NULL;
                    (*edge_p2p)->NodePtr = previous_node;
                    (*edge_p2p)->EdgeCov++;
                }
            }
        }

        previous_node = current_node;

        if (query->tAlignedSeq[i] != '-') {
            target_position++;
        }
    }
}

void BFSFindBestPath(Backbone* backbone, int node_idx)
{
    PtrNode begin_node = backbone->Nodes[node_idx];
    std::list<PtrNode> node_list;
    node_list.push_back(begin_node);
    int max_cov = backbone->Covs[node_idx];
    while (node_list.size() > 0) {
        PtrNode current_node = node_list.front();
        node_list.pop_front();
        PtrEdgeNode edge_ptr = current_node->Right;
        while (edge_ptr != NULL) {
            bool update = 0;
            int new_score = 0;
            if (backbone->Threshold < 0.0) {
                new_score =
                    (current_node->Score + std::max(edge_ptr->EdgeCov - backbone->MinCov, -2));
                if (edge_ptr->NodePtr->Score < new_score) {
                    update = 1;
                }
            } else {
                int threshold =
                    std::max(static_cast<int32_t>(std::round((max_cov * backbone->Threshold))),
                             backbone->MinCov);
                new_score = (current_node->Score + edge_ptr->EdgeCov - threshold);
                if (edge_ptr->NodePtr->Score < new_score) {
                    update = 1;
                }
            }

            if (update) {
                edge_ptr->NodePtr->Cov = current_node->Cov + 1;

                edge_ptr->NodePtr->Score = new_score;

                edge_ptr->NodePtr->LastNode = current_node;
                if (!edge_ptr->NodePtr->InBackbone) {
                    node_list.push_back(edge_ptr->NodePtr);
                }
            }

            edge_ptr = edge_ptr->NextEdge;
        }
    }
}

void BFSFindBestPathSparse(Backbone* backbone, int node_idx)
{
    PtrNode begin_node = backbone->Nodes[node_idx];
    std::list<PtrNode> node_list;
    node_list.push_back(begin_node);
    int max_cov = backbone->Covs[node_idx];
    while (node_list.size() > 0) {
        PtrNode current_node = node_list.front();
        node_list.pop_front();
        PtrEdgeNode edge_ptr = current_node->Right;

        while (edge_ptr != NULL) {
            bool update = 0;

            int new_score = 0;

            if (backbone->Threshold < 0.0) {
                new_score = current_node->Score +
                            edge_ptr->Len * std::max(edge_ptr->EdgeCov - backbone->MinCov, -2);
                if (edge_ptr->NodePtr->Score < new_score) {
                    update = 1;
                }
            } else {
                int threshold =
                    std::max(static_cast<int32_t>(std::round((max_cov * backbone->Threshold))),
                             backbone->MinCov);
                new_score = (current_node->Score + edge_ptr->Len * (edge_ptr->EdgeCov - threshold));
                if (edge_ptr->NodePtr->Score < new_score) {
                    update = 1;
                }
            }

            if (update) {
                edge_ptr->NodePtr->Cov = current_node->Cov + 1;
                edge_ptr->NodePtr->Score = new_score;

                edge_ptr->NodePtr->LastNode = current_node;
                if (!edge_ptr->NodePtr->InBackbone) {
                    node_list.push_back(edge_ptr->NodePtr);
                }
            }

            edge_ptr = edge_ptr->NextEdge;
        }
    }
}

void FindBestPath(Backbone* backbone)
{
    backbone->Nodes[0]->Cov = 1;  //depth
    for (size_t i = 0; i + 1 < backbone->Nodes.size(); ++i) {

        BFSFindBestPath(backbone, i);

        PtrEdgeNode edge_ptr = backbone->Nodes[i]->Right;
        std::map<uint64_t, PtrNode> node_map;
        std::map<PtrNode, int> node_cov;
        while (edge_ptr != NULL) {
            edge_ptr = edge_ptr->NextEdge;
        }

        node_map.clear();
        node_cov.clear();

        edge_ptr = backbone->Nodes[i]->Left;
        while (edge_ptr != NULL) {
            edge_ptr = edge_ptr->NextEdge;
        }
    }
}

void FindBestPathSparse(Backbone* backbone)
{
    backbone->Nodes[0]->Cov = 1;  //depth
    for (size_t i = 0; i + 1 < backbone->Nodes.size(); ++i) {
        if (backbone->Nodes[i] == NULL) {
            continue;
        }

        BFSFindBestPathSparse(backbone, i);

        PtrEdgeNode edge_ptr = backbone->Nodes[i]->Right;
        std::map<uint64_t, PtrNode> node_map;
        std::map<PtrNode, int> node_cov;
        while (edge_ptr != NULL) {
            edge_ptr = edge_ptr->NextEdge;
        }

        node_map.clear();
        node_cov.clear();

        edge_ptr = backbone->Nodes[i]->Left;
        while (edge_ptr != NULL) {
            edge_ptr = edge_ptr->NextEdge;
        }
    }
}
void AddPathToBackboneSparse(Backbone& bb, Query& qq, size_t k, EdgeBucket& edgeBucket,
                             NodeBucket& nodeBucket)
{
    int gap = bb.Gap;

    std::string t1, q1;
    NormalizeAlignment(&qq);
    std::string t2, q2;

    PtrNode current_node = NULL;
    int qStart = -1;
    std::vector<bool> matched;
    matched.resize(qq.qAlignedSeq.size());
    std::vector<int> Align2Backbone, Align2Query, Query2Align;

    Align2Backbone.resize(qq.qAlignedSeq.size());
    Align2Query.resize(qq.qAlignedSeq.size());
    Query2Align.resize(qq.qAlignedSeq.size());
    std::string processed_query, processed_target;
    processed_query.resize(qq.qAlignedSeq.size());
    processed_target.resize(qq.qAlignedSeq.size());
    int n_t = 0, n_q = 0;

    for (size_t i = 0; i < qq.qAlignedSeq.size(); ++i) {
        if (qq.qAlignedSeq[i] != '-') {
            processed_query[n_q] = (qq.qAlignedSeq[i]);
            n_q++;
        }
        if (qq.tAlignedSeq[i] != '-') {
            processed_target[n_t] = (qq.tAlignedSeq[i]);
            n_t++;
        }
    }
    processed_query.resize(n_q);
    processed_target.resize(n_t);

    int base_cnt = 0;
    for (size_t i = 0; i < qq.qAlignedSeq.size(); ++i) {
        Align2Query[i] = base_cnt;
        if (qq.qAlignedSeq[i] != '-') {
            Query2Align[base_cnt] = i;
            base_cnt++;
        }
    }

    base_cnt = qq.tStart;
    for (size_t i = 0; i + k <= qq.qAlignedSeq.size(); ++i) {
        Align2Backbone[i] = base_cnt;
        if (qq.tAlignedSeq[i] != '-') {
            base_cnt++;
        }
    }

    for (size_t i = 0; i < qq.qAlignedSeq.size(); ++i) {
        matched[i] = 0;
    }

    for (size_t i = 0; i + k <= qq.tAlignedSeq.size(); ++i) {
        bool match = 1;
        size_t match_bases = 0;
        for (size_t j = 0; j < qq.tAlignedSeq.size(); ++j) {
            if (qq.qAlignedSeq[i + j] == qq.tAlignedSeq[i + j]) {
                if (qq.qAlignedSeq[i + j] != '-') {
                    match_bases++;
                }
                if (match_bases == k) {
                    break;
                }

            } else {
                match = 0;
                break;
            }
        }

        matched[i] = match;
        if (qStart < 0 && match) {
            qStart = i;
        }
    }

    if (qStart < 0) {
        return;
    }

    if (bb.Nodes[Align2Backbone[qStart]] != NULL) {
        current_node = bb.Nodes[Align2Backbone[qStart]];
    } else {
        std::string KmerStr = processed_query.substr(Align2Query[qStart], k);

        if (KmerStr.size() != k) {
            return;
        }
        uint64_t seq = 0;
        Str2BitArray(KmerStr, k, &seq, 1);
        current_node = new Node();
        nodeBucket.Add(current_node);
        current_node->Kmer = static_cast<uint32_t>(seq);
        bb.Nodes[Align2Backbone[qStart]] = current_node;
        current_node->InBackbone = true;
    }

    for (size_t i = qStart; i < qq.qAlignedSeq.size();) {
        //explore the edges of the current node

        if (qq.qAlignedSeq[i] == '-') {
            i++;  // must skip to promise the correctness of the algorithm
            continue;
        }
        current_node->Cov++;  //increase the coverage
        std::string EdgeStr;
        EdgeStr = processed_query.substr(Align2Query[i] + k, gap);

        if (EdgeStr.size() == 0) {

            return;
        }
        PtrEdgeNode current_edge = current_node->Right;
        bool edge_exists = 0;
        while (current_edge != NULL) {
            if (EdgeStr.size() < current_edge->Len) {
                current_edge = current_edge->NextEdge;
                continue;
            }
            uint64_t edge_bits;
            Str2BitArray(EdgeStr, current_edge->Len, &edge_bits, 1);

            if (edge_bits == current_edge->Edge) {

                bool edge_matched = 0;

                int align_idx = Query2Align[Align2Query[i] + current_edge->Len];
                if (matched[align_idx] &&
                    bb.Nodes[Align2Backbone[align_idx]] == current_edge->NodePtr) {
                    edge_matched = 1;  //backbone matched, so move on
                }
                if (!matched[align_idx] && !current_edge->NodePtr->InBackbone) {
                    edge_matched = 1;  //off-backbone branch matched, so move on
                }

                if (edge_matched) {
                    current_edge->EdgeCov++;

                    edge_exists = 1;
                    current_node = current_edge->NodePtr;

                    std::string LeftEdgeStr =
                        processed_query.substr(Align2Query[i], current_edge->Len);

                    Str2BitArray(LeftEdgeStr, current_edge->Len, &edge_bits, 1);
                    PtrEdgeNode left_edge = current_node->Left;
                    while (left_edge != NULL) {
                        if (edge_bits == left_edge->Edge &&
                            LeftEdgeStr.size() == current_edge->Len) {
                            break;
                        }

                        left_edge = left_edge->NextEdge;
                    }
                    if (left_edge != NULL) {
                        left_edge->EdgeCov++;
                    } else {
                        throw std::runtime_error("left edge error.");
                        return;
                    }

                    i = align_idx;
                    break;
                }
            }

            current_edge = current_edge->NextEdge;
        }

        if (edge_exists) {
            continue;
        }

        //There is a backbone match but was not recorded, or there is no match.

        /////non-matching, so pick the best stretch
        // case 1, within g bases, there is a match with the backbone -- put the node into the backbone nodes
        // otherwise, skip g bases, and create a new node.

        current_node->Cov++;
        bool backbone_match = 0;

        int jj;                   //jj is the position in the processed query read
        int ii = Align2Query[i];  // these two lines are very tricky,
        int align_idx = -1;
        for (jj = ii + 1; jj <= ii + gap; ++jj) {
            align_idx = Query2Align[jj];
            if (matched[align_idx]) {
                backbone_match = 1;
                break;
            }
            if (jj + k >= processed_query.size()) {
                break;
            }
        }
        jj = std::min(jj, ii + gap);

        std::string temp_str;
        temp_str = processed_query.substr(ii, jj - ii + k);

        if (temp_str.size() != jj - ii + k) {
            //std::cout << "bug. skipped" << std::endl;
            return;
        }

        PtrNode next_node;

        // Out of bound, bug
        if (Align2Backbone[align_idx] >= static_cast<int32_t>(bb.Nodes.size())) {
            return;
        }

        if (backbone_match && bb.Nodes[Align2Backbone[align_idx]] != NULL) {
            //add edges to the existing backbone node.
            next_node = bb.Nodes[Align2Backbone[align_idx]];
            if (next_node == current_node) {
                //std::cout << "bug caught, linked to itself." << std::endl;
                return;
            }

        } else {
            //allocate a new node and new edges
            std::string kmer_str = temp_str.substr(temp_str.size() - k, k);
            next_node = new Node();
            nodeBucket.Add(next_node);
            uint64_t seq;
            Str2BitArray(kmer_str, k, &seq, 1);
            next_node->Kmer = static_cast<uint32_t>(seq);
            if (backbone_match) {
                bb.Nodes[Align2Backbone[align_idx]] = next_node;
                next_node->InBackbone = true;
            }
        }

        //append edges, quite tricky
        std::string edge_str;
        edge_str = temp_str.substr(k, temp_str.size());

        uint64_t edge_bits;
        Str2BitArray(edge_str, edge_str.size(), &edge_bits, 1);

        PtrEdgeNode* edge_p2p = &(current_node->Right);

        while (*edge_p2p != NULL) {
            edge_p2p = &((*edge_p2p)->NextEdge);
        }

        if (*edge_p2p == NULL) {

            (*edge_p2p) = new EdgeNode();
            edgeBucket.Add(*edge_p2p);
            (*edge_p2p)->Edge = static_cast<uint32_t>(edge_bits);
            (*edge_p2p)->Len = edge_str.size();
            (*edge_p2p)->NextEdge = NULL;
            (*edge_p2p)->NodePtr = next_node;
            (*edge_p2p)->EdgeCov++;
        }

        edge_str = temp_str.substr(0, temp_str.size() - k);
        Str2BitArray(edge_str, edge_str.size(), &edge_bits, 1);

        edge_p2p = &(next_node->Left);

        while (*edge_p2p != NULL) {

            edge_p2p = &((*edge_p2p)->NextEdge);
        }

        if (*edge_p2p == NULL) {

            (*edge_p2p) = new EdgeNode();
            edgeBucket.Add(*edge_p2p);
            (*edge_p2p)->Edge = static_cast<uint32_t>(edge_bits);
            (*edge_p2p)->Len = edge_str.size();
            (*edge_p2p)->NextEdge = NULL;
            (*edge_p2p)->NodePtr = current_node;
            (*edge_p2p)->EdgeCov++;
        }
        uint64_t kmer_bits = current_node->Kmer;

        char kmer_str[100];
        BitsArray2Str(&kmer_bits, k, kmer_str);
        if ((*edge_p2p)->Len == k && strcmp(kmer_str, edge_str.c_str()) != 0) {
            /*
            std::cout << "bug" << std::endl;
            std::cout << kmer_str << std::endl;
            std::cout << edge_str << std::endl;
            std::abort();
            */
            throw std::runtime_error("Unknown error");
        }

        current_node = next_node;
        i = align_idx;
    }
}
}  // namespace

std::string SparcConsensus(const std::string& backboneSeq, std::vector<Query>& queries,
                           const int32_t gap, const size_t k, const int32_t minCov,
                           const double threshold)
{
    const int32_t backboneSeqLen = backboneSeq.size();

    Read ref;
    ref.ReadBits.resize((backboneSeqLen / 4) + 100);
    InitRefRead(backboneSeq, ref);

    Backbone backbone;
    backbone.Threshold = threshold;
    backbone.Gap = gap;
    backbone.MinCov = minCov;

    EdgeBucket edgeBucket;
    NodeBucket nodeBucket;

    if (gap == 1) {
        ConsensusKmerGraphConstruction(&ref, &backbone, k, edgeBucket, nodeBucket);
    } else {
        ConsensusSparseKmerGraphConstruction(&ref, &backbone, k, edgeBucket, nodeBucket);
    }

    for (Query& q : queries) {
        if (gap == 1) {
            AddPathToBackbone(&backbone, &q, k, edgeBucket, nodeBucket);
        } else {
            AddPathToBackboneSparse(backbone, q, k, edgeBucket, nodeBucket);
        }
    }

    // hopefully never goes zero
    size_t radius = std::min(std::min<size_t>(200, backbone.Nodes.size()),
                             static_cast<size_t>(backboneSeqLen - 1));
    std::map<int, int> cov_cnt;
    backbone.Covs.resize(backbone.Nodes.size());

    for (size_t i = 0; i < radius; ++i) {
        if (backbone.Nodes[i] != NULL) {
            cov_cnt[backbone.Nodes[i]->Cov]++;
        }
    }

    for (size_t i = 0; i < backbone.Nodes.size(); ++i) {
        if (i >= radius) {
            if (backbone.Nodes[i - radius] != NULL) {
                cov_cnt[backbone.Nodes[i - radius]->Cov]--;
                if (cov_cnt[backbone.Nodes[i - radius]->Cov] <= 0) {
                    cov_cnt.erase(backbone.Nodes[i - radius]->Cov);
                }
            }
        }
        if ((i + radius < backbone.Nodes.size()) && (backbone.Nodes[i + radius] != NULL)) {
            cov_cnt[backbone.Nodes[i + radius]->Cov]++;
        }

        if (cov_cnt.size() > 0) {
            backbone.Covs[i] = cov_cnt.rbegin()->first;
        } else {
            backbone.Covs[i] = 0;
        }
    }

    if (gap == 1) {
        FindBestPath(&backbone);
    } else {
        FindBestPathSparse(&backbone);
    }

    std::string consensus;
    if (gap == 1) {
        int64_t max_score = 0;
        size_t position = 0;

        for (size_t i = 0; i < backbone.Nodes.size(); ++i) {
            if (backbone.Nodes[i]->Score > max_score) {
                max_score = backbone.Nodes[i]->Score;
                position = i;
            }
            backbone.Nodes[i]->InBackbone = false;
        }

        if (max_score == 0) {
            return "";
        }

        PtrNode current_node = backbone.Nodes[position];

        char KmerStr[5];
        if (current_node != NULL) {
            uint64_t kmer_uint64 = (current_node->Kmer);
            BitsArray2Str(&kmer_uint64, k, KmerStr);
            consensus = KmerStr;
            std::reverse(consensus.begin(), consensus.end());
            current_node = current_node->LastNode;
            while (current_node != NULL) {
                kmer_uint64 = (current_node->Kmer);
                BitsArray2Str(&kmer_uint64, k, KmerStr);

                consensus.push_back(KmerStr[0]);
                current_node = current_node->LastNode;
            }

            std::reverse(consensus.begin(), consensus.end());
        }

        current_node = backbone.Nodes[position];
        backbone.Nodes.clear();
        backbone.Nodes.push_back(current_node);
        if (current_node != NULL) {
            current_node = current_node->LastNode;
            while (current_node != NULL) {
                backbone.Nodes.push_back(current_node);
                current_node = current_node->LastNode;
            }
        }
        std::reverse(backbone.Nodes.begin(), backbone.Nodes.end());

        for (size_t ii = 0; ii < backbone.Nodes.size(); ++ii) {
            backbone.Nodes[ii]->InBackbone = true;
        }
    } else {
        int64_t max_score = 0;
        size_t position = 0;
        for (size_t i = 0; i < backbone.Nodes.size(); ++i) {
            if (backbone.Nodes[i] != NULL && backbone.Nodes[i]->Score > max_score) {
                max_score = backbone.Nodes[i]->Score;
                position = i;
            }
            if (backbone.Nodes[i] != NULL) {
                backbone.Nodes[i]->InBackbone = false;
            }
        }

        PtrNode current_node = backbone.Nodes[position];
        backbone.Nodes.clear();
        backbone.Nodes.push_back(current_node);
        if (max_score == 0) {
            return "";
        }
        char KmerStr[5];
        if (current_node != NULL) {
            uint64_t kmer_uint64 = (current_node->Kmer);
            BitsArray2Str(&kmer_uint64, k, KmerStr);
            consensus = KmerStr;
            std::reverse(consensus.begin(), consensus.end());
            PtrEdgeNode edge_node = current_node->Left;
            PtrEdgeNode best_edge_node = NULL;
            int max_edge_cov = 0;
            while (edge_node != NULL) {
                if (edge_node->NodePtr == current_node->LastNode) {
                    if (edge_node->EdgeCov >= max_edge_cov) {
                        max_edge_cov = edge_node->EdgeCov;
                        best_edge_node = edge_node;
                    }
                }
                edge_node = edge_node->NextEdge;
            }
            uint64_t edge_bits = best_edge_node->Edge;

            char edge_str[5];
            BitsArray2Str(&edge_bits, best_edge_node->Len, edge_str);
            for (int i = best_edge_node->Len - 1; i >= 0; --i) {
                consensus.push_back(edge_str[i]);
                if (i > 0) {
                    backbone.Nodes.push_back(NULL);
                }
            }

            current_node = current_node->LastNode;

            while (current_node->LastNode != NULL) {
                backbone.Nodes.push_back(current_node);
                edge_node = current_node->Left;
                best_edge_node = NULL;
                max_edge_cov = 0;
                while (edge_node != NULL) {
                    if (edge_node->NodePtr == current_node->LastNode) {
                        if (edge_node->EdgeCov > max_edge_cov || max_edge_cov == 0) {
                            max_edge_cov = edge_node->EdgeCov;
                            best_edge_node = edge_node;
                        }
                    }
                    edge_node = edge_node->NextEdge;
                }

                uint64_t edge_bits_local = best_edge_node->Edge;

                char edge_str_local[5];
                BitsArray2Str(&edge_bits_local, best_edge_node->Len, edge_str_local);

                uint64_t kmer_bits = current_node->LastNode->Kmer;

                char kmer_str[5];
                BitsArray2Str(&kmer_bits, k, kmer_str);
                if (best_edge_node->Len == k && strcmp(kmer_str, edge_str_local) != 0) {
                    throw std::runtime_error("Unknown error");
                }
                for (int i = best_edge_node->Len - 1; i >= 0; --i) {
                    consensus.push_back(edge_str_local[i]);
                    if (i > 0) {
                        backbone.Nodes.push_back(NULL);
                    }
                }

                current_node = current_node->LastNode;
            }

            std::reverse(consensus.begin(), consensus.end());
            std::reverse(backbone.Nodes.begin(), backbone.Nodes.end());

            for (size_t ii = 0; ii < backbone.Nodes.size(); ++ii) {
                if (backbone.Nodes[ii] != NULL) {
                    backbone.Nodes[ii]->InBackbone = true;
                }
            }
        }
    }

    return consensus;
}
}  // namespace Sparc

#include <pbcopper/qgram/Index.h>

#include <gtest/gtest.h>

TEST(QGram_Index, shape_throws_on_invalid_qgram_sizes)
{
    EXPECT_THROW(PacBio::QGram::internal::Shape(0, "ACGTACGT"), std::invalid_argument);
    EXPECT_THROW(PacBio::QGram::internal::Shape(20, "ACGTACGT"), std::invalid_argument);
}

TEST(QGram_Index, shape_hash_factors)
{
    //4^(q-1)

    PacBio::QGram::internal::Shape shape_3{3, "ACGTACGT"};
    EXPECT_EQ(16, shape_3.hashFactor_);

    PacBio::QGram::internal::Shape shape_6{6, "ACGTACGT"};
    EXPECT_EQ(1024, shape_6.hashFactor_);
}

TEST(QGram_Index, shape_hashing)
{
    auto check = [](const std::size_t q, std::vector<uint64_t> expected) {
        ASSERT_EQ(4, expected.size());

        const std::string seqA(q, 'A');
        const std::string seqC(q, 'C');
        const std::string seqG(q, 'G');
        const std::string seqT(q, 'T');

        PacBio::QGram::internal::Shape shapeA{q, seqA};
        PacBio::QGram::internal::Shape shapeC{q, seqC};
        PacBio::QGram::internal::Shape shapeG{q, seqG};
        PacBio::QGram::internal::Shape shapeT{q, seqT};

        EXPECT_EQ(expected.at(0), shapeA.HashNext());
        EXPECT_EQ(expected.at(1), shapeC.HashNext());
        EXPECT_EQ(expected.at(2), shapeG.HashNext());
        EXPECT_EQ(expected.at(3), shapeT.HashNext());
    };

    // (homopolymer) sequence of size(q) yields expected hashes
    check(3, {0, 21, 42, 63});
    check(6, {0, 1365, 2730, 4095});
    check(10, {0, 349525, 699050, 1048575});
}

TEST(QGram_Index, homopolymer_hasher)
{
    using HashHit = std::pair<uint64_t, bool>;

    // clang-format off
    const std::vector<HashHit> input
    {
        { 0,  true  },
        { 0,  true  },
        { 1,  false },
        { 4,  false },
        { 17, false },
        { 4,  false },
        { 18, false },
        { 11, false },
        { 47, false },
        { 63, true  },
        { 62, false },
        { 56, false }
    };
    // clang-format on

    const auto isHomopolymer = PacBio::QGram::internal::HpHasher{3};
    for (const auto& hit : input) {
        EXPECT_EQ(hit.second, isHomopolymer(hit.first));
    }
}

TEST(QGram_Index, shape_hash_iteration)
{
    using HashHit = std::pair<uint64_t, bool>;

    // clang-format off
    const std::vector<HashHit> input
    {
        { 0,  true  },
        { 0,  true  },
        { 1,  false },
        { 4,  false },
        { 17, false },
        { 4,  false },
        { 18, false },
        { 11, false },
        { 47, false },
        { 63, true  },
        { 62, false },
        { 56, false }
    };
    // clang-format on

    const std::string text{"AAAACACAGTTTGA"};

    PacBio::QGram::internal::Shape shape{3, text};
    ASSERT_TRUE(input.size() <= text.size());
    const auto isHomopolymer = PacBio::QGram::internal::HpHasher{3};
    for (unsigned i = 0; i < text.size() - 3 + 1; ++i) {
        const auto h = shape.HashNext();
        const auto b = isHomopolymer(h);
        EXPECT_EQ(input[i].first, h);
        EXPECT_EQ(input[i].second, b);
    }
}

TEST(QGram_Index, index_construct)
{
    auto check = [](const PacBio::QGram::internal::IndexImpl& index,
                    const uint32_t expectedQGramCount, const std::size_t expectedDirLength,
                    const PacBio::QGram::internal::IndexImpl::HashLookup_t& expectedHL,
                    const PacBio::QGram::internal::IndexImpl::SuffixArray_t& expectedSA) {
        const auto& sa = index.SuffixArray();
        const auto& dir = index.HashLookup();

        EXPECT_EQ(expectedQGramCount, sa.size());
        EXPECT_EQ(expectedDirLength, dir.size());
        EXPECT_EQ(expectedDirLength, expectedHL.size());
        EXPECT_EQ(expectedQGramCount, expectedSA.size());

        EXPECT_EQ(expectedHL, dir);
        EXPECT_EQ(expectedSA, sa);
    };

    {
        SCOPED_TRACE("single (short)");
        const PacBio::QGram::internal::IndexImpl index{3, {"CATGATTACATA"}};
        check(index, 10, 65,
              PacBio::QGram::internal::IndexImpl::HashLookup_t{
                  0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 4,  4,  4,  6, 6,
                  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7,  7,  7,  7, 7,
                  7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10},
              PacBio::QGram::internal::IndexImpl::SuffixArray_t{
                  {0, 7}, {0, 9}, {0, 1}, {0, 4}, {0, 0}, {0, 8}, {0, 3}, {0, 6}, {0, 2}, {0, 5}});
    }
    {
        SCOPED_TRACE("single (longer)");
        const PacBio::QGram::internal::IndexImpl index{3, {"CATGATTACATACATGATTACATA"}};
        check(index, 22, 65,
              PacBio::QGram::internal::IndexImpl::HashLookup_t{
                  0,  0,  0,  0,  0,  3,  3,  3,  3,  3,  3,  3,  3,  5,  5,  7,  9,
                  9,  9,  9,  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
                  13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 18,
                  18, 18, 18, 18, 18, 18, 20, 20, 20, 20, 22, 22, 22, 22},
              PacBio::QGram::internal::IndexImpl::SuffixArray_t{
                  {0, 7},  {0, 11}, {0, 19}, {0, 9},  {0, 21}, {0, 1}, {0, 13}, {0, 4},
                  {0, 16}, {0, 0},  {0, 8},  {0, 12}, {0, 20}, {0, 3}, {0, 15}, {0, 6},
                  {0, 10}, {0, 18}, {0, 2},  {0, 14}, {0, 5},  {0, 17}});
    }
    {
        SCOPED_TRACE("multi - duplicate (short)");
        const std::vector<std::string> seqs{"CATGATTACATA", "CATGATTACATA"};
        const PacBio::QGram::internal::IndexImpl index{3, seqs};
        check(index, 20, 65,
              PacBio::QGram::internal::IndexImpl::HashLookup_t{
                  0,  0,  0,  0,  0,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  6,  8,
                  8,  8,  8,  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                  12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 16,
                  16, 16, 16, 16, 16, 16, 18, 18, 18, 18, 20, 20, 20, 20},
              PacBio::QGram::internal::IndexImpl::SuffixArray_t{
                  {0, 7}, {1, 7}, {0, 9}, {1, 9}, {0, 1}, {1, 1}, {0, 4}, {1, 4}, {0, 0}, {0, 8},
                  {1, 0}, {1, 8}, {0, 3}, {1, 3}, {0, 6}, {1, 6}, {0, 2}, {1, 2}, {0, 5}, {1, 5},
              });
    }
    {
        SCOPED_TRACE("multi - duplicate (longer))");
        const std::vector<std::string> seqs{"CATGATTACATACATGATTACATA", "CATGATTACATACATGATTACATA"};
        const PacBio::QGram::internal::IndexImpl index{3, seqs};
        check(index, 44, 65,
              PacBio::QGram::internal::IndexImpl::HashLookup_t{
                  0,  0,  0,  0,  0,  6,  6,  6,  6,  6,  6,  6,  6,  10, 10, 14, 18,
                  18, 18, 18, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
                  26, 26, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 36,
                  36, 36, 36, 36, 36, 36, 40, 40, 40, 40, 44, 44, 44, 44},
              PacBio::QGram::internal::IndexImpl::SuffixArray_t{
                  {0, 7},  {0, 11}, {0, 19}, {1, 7},  {1, 11}, {1, 19}, {0, 9},  {0, 21}, {1, 9},
                  {1, 21}, {0, 1},  {0, 13}, {1, 1},  {1, 13}, {0, 4},  {0, 16}, {1, 4},  {1, 16},
                  {0, 0},  {0, 8},  {0, 12}, {0, 20}, {1, 0},  {1, 8},  {1, 12}, {1, 20}, {0, 3},
                  {0, 15}, {1, 3},  {1, 15}, {0, 6},  {0, 10}, {0, 18}, {1, 6},  {1, 10}, {1, 18},
                  {0, 2},  {0, 14}, {1, 2},  {1, 14}, {0, 5},  {0, 17}, {1, 5},  {1, 17}});
    }
    {
        SCOPED_TRACE("multi - unique (short)");
        const std::vector<std::string> seqs{"CATGATTACATA", "TTAGATAACTTC"};
        const PacBio::QGram::internal::IndexImpl index{3, seqs};
        check(index, 20, 65,
              PacBio::QGram::internal::IndexImpl::HashLookup_t{
                  0,  0,  1,  1,  1,  2,  2,  2,  3,  4,  4,  4,  4,  6,  6,  7,  8,
                  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11,
                  11, 11, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 15,
                  16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 19, 20, 20, 20},
              PacBio::QGram::internal::IndexImpl::SuffixArray_t{
                  {1, 6}, {0, 7}, {1, 7}, {1, 2}, {0, 9}, {1, 4}, {0, 1}, {0, 4}, {0, 0}, {0, 8},
                  {1, 8}, {0, 3}, {1, 3}, {1, 5}, {0, 6}, {1, 1}, {0, 2}, {0, 5}, {1, 0}, {1, 9}});
    }
    {
        SCOPED_TRACE("multi - unique (longer)");
        const std::vector<std::string> seqs{"CATGATTACATACATGATTACATA", "TTAGATAACTTCTTAGATAACTTC"};
        const PacBio::QGram::internal::IndexImpl index{3, seqs};
        check(index, 44, 65,
              PacBio::QGram::internal::IndexImpl::HashLookup_t{
                  0,  0,  2,  2,  2,  5,  5,  5,  7,  9,  9,  9,  9,  13, 13, 15, 17,
                  17, 17, 17, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 24, 24,
                  24, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 30, 33,
                  35, 35, 35, 35, 35, 36, 38, 38, 38, 38, 42, 44, 44, 44},
              PacBio::QGram::internal::IndexImpl::SuffixArray_t{
                  {1, 6},  {1, 18}, {0, 7},  {0, 11}, {0, 19}, {1, 7},  {1, 19}, {1, 2},  {1, 14},
                  {0, 9},  {0, 21}, {1, 4},  {1, 16}, {0, 1},  {0, 13}, {0, 4},  {0, 16}, {0, 0},
                  {0, 8},  {0, 12}, {0, 20}, {1, 8},  {1, 11}, {1, 20}, {0, 3},  {0, 15}, {1, 3},
                  {1, 15}, {1, 5},  {1, 17}, {0, 6},  {0, 10}, {0, 18}, {1, 1},  {1, 13}, {1, 10},
                  {0, 2},  {0, 14}, {0, 5},  {0, 17}, {1, 0},  {1, 12}, {1, 9},  {1, 21}});
    }
}

TEST(QGram_Index, index_copy)
{
    const std::vector<uint64_t> expected{118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                                         128, 129, 130, 131, 51,  132, 133, 134, 135, 266,
                                         136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
                                         146, 147, 148, 149, 150, 151, 152, 153, 154};

    const std::string inputSeq{
        "TCCAACTTAGGCATAAACCTGCATGCTACCTTGTCAGACCCACTCTGCACGAAGTAA"
        "ATATGGGATGCGTCCGACCTGGCTCCTGGCGTTCCACGCCGCCACGTGTTCGTTAAC"
        "TGTTGATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGGCTCAGTTACTT"
        "CGAGCGTAATGTCTCAAATGGCGTAGAACGGCAATGACTGTTTGACACTAGGTGGTG"
        "TTCAGTTCGGTAACGGAGAGTCTGTGCGGCATTCTTATTAATACATTTGAAACGCGC"
        "CCAACTGACGCTAGGCAAGTCAGTGCAGGCTCCCGTGTTAGGATAAGGGTAAACATA"
        "CAAGTCGATAGAAGATGGGTAGGGGCCTTCAATTCATCCAGCACTCTACGGTTCCTC"
        "CGAGAGCAAGTAGGGCACCCTGTAGTTCGAAGGGGAACTATTTCGTGGGGCGAGCCC"
        "ACACCGTCTCTTCTGCGGAAGACTTAACACGTTAGGGAGGTGGA"};

    const std::string query{"GATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGG"};

    auto checkIndexHits = [](const PacBio::QGram::Index& index, const std::string& querySeq,
                             const std::vector<uint64_t>& expectedVec) {
        std::vector<uint64_t> observed;
        for (const auto& hits : index.Hits(querySeq)) {
            for (const auto& hit : hits) {
                EXPECT_EQ(0, hit.Id());
                observed.push_back(hit.Position());
            }
        }
        EXPECT_EQ(expectedVec, observed);
    };

    const PacBio::QGram::Index idx{6, inputSeq};
    const PacBio::QGram::Index& idxCopy{idx};
    const PacBio::QGram::Index& idxAssign = idx;

    checkIndexHits(idx, query, expected);
    checkIndexHits(idxCopy, query, expected);
    checkIndexHits(idxAssign, query, expected);
}

TEST(QGram_Index, index_hits_INTERNAL_from_shape_short_seq)
{
    const std::vector<PacBio::QGram::IndexHit> expected{{0, 0}, {0, 8}};

    const PacBio::QGram::internal::IndexImpl idx(3, {"CATGATTACATA"});
    const std::string seq{"CAT"};
    const auto q = idx.Size();
    auto shape = PacBio::QGram::internal::Shape{q, seq};
    const auto isHomopolymer = PacBio::QGram::internal::HpHasher{q};
    const auto end = PacBio::Utility::SafeSubtract(seq.size() + 1, q);

    for (uint32_t i = 0; i < end; ++i) {
        if (isHomopolymer(shape.HashNext())) {
            continue;
        }
        const auto hits = idx.Hits(shape, i);
        ASSERT_EQ(expected.size(), hits.size());
        for (std::size_t j = 0; j < hits.size(); ++j) {
            const auto& hit = hits.at(j);
            EXPECT_EQ(expected.at(j).Id(), hit.Id());
            EXPECT_EQ(expected.at(j).Position(), hit.Position());
        }
    }
}

TEST(QGram_Index, index_hits_INTERNAL_from_shape_longer_seq)
{
    const std::vector<uint64_t> expected{118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                                         128, 129, 130, 131, 51,  132, 133, 134, 135, 266,
                                         136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
                                         146, 147, 148, 149, 150, 151, 152, 153, 154};
    const PacBio::QGram::internal::IndexImpl idx{
        6,
        {"TCCAACTTAGGCATAAACCTGCATGCTACCTTGTCAGACCCACTCTGCACGAAGTAA"
         "ATATGGGATGCGTCCGACCTGGCTCCTGGCGTTCCACGCCGCCACGTGTTCGTTAAC"
         "TGTTGATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGGCTCAGTTACTT"
         "CGAGCGTAATGTCTCAAATGGCGTAGAACGGCAATGACTGTTTGACACTAGGTGGTG"
         "TTCAGTTCGGTAACGGAGAGTCTGTGCGGCATTCTTATTAATACATTTGAAACGCGC"
         "CCAACTGACGCTAGGCAAGTCAGTGCAGGCTCCCGTGTTAGGATAAGGGTAAACATA"
         "CAAGTCGATAGAAGATGGGTAGGGGCCTTCAATTCATCCAGCACTCTACGGTTCCTC"
         "CGAGAGCAAGTAGGGCACCCTGTAGTTCGAAGGGGAACTATTTCGTGGGGCGAGCCC"
         "ACACCGTCTCTTCTGCGGAAGACTTAACACGTTAGGGAGGTGGA"}};

    const std::string seq{"GATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGG"};

    std::vector<uint64_t> observed;

    const auto q = idx.Size();
    auto shape = PacBio::QGram::internal::Shape{q, seq};
    const auto end = PacBio::Utility::SafeSubtract(seq.size() + 1, q);
    for (uint32_t j = 0; j < end; ++j) {
        shape.HashNext();
        const auto hits = idx.Hits(shape, j);
        for (std::size_t i = 0; i < hits.size(); ++i) {
            observed.push_back(hits.at(i).Position());
        }
    }

    EXPECT_EQ(expected, observed);
}

TEST(QGram_Index, index_hits_PUBLIC_API_from_seq)
{
    const std::vector<uint64_t> expected{118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                                         128, 129, 130, 131, 51,  132, 133, 134, 135, 266,
                                         136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
                                         146, 147, 148, 149, 150, 151, 152, 153, 154};

    const PacBio::QGram::Index idx{6,
                                   "TCCAACTTAGGCATAAACCTGCATGCTACCTTGTCAGACCCACTCTGCACGAAGTAA"
                                   "ATATGGGATGCGTCCGACCTGGCTCCTGGCGTTCCACGCCGCCACGTGTTCGTTAAC"
                                   "TGTTGATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGGCTCAGTTACTT"
                                   "CGAGCGTAATGTCTCAAATGGCGTAGAACGGCAATGACTGTTTGACACTAGGTGGTG"
                                   "TTCAGTTCGGTAACGGAGAGTCTGTGCGGCATTCTTATTAATACATTTGAAACGCGC"
                                   "CCAACTGACGCTAGGCAAGTCAGTGCAGGCTCCCGTGTTAGGATAAGGGTAAACATA"
                                   "CAAGTCGATAGAAGATGGGTAGGGGCCTTCAATTCATCCAGCACTCTACGGTTCCTC"
                                   "CGAGAGCAAGTAGGGCACCCTGTAGTTCGAAGGGGAACTATTTCGTGGGGCGAGCCC"
                                   "ACACCGTCTCTTCTGCGGAAGACTTAACACGTTAGGGAGGTGGA"};

    const std::string seq{"GATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGG"};

    std::vector<uint64_t> observed;
    for (const auto& hits : idx.Hits(seq)) {
        for (const auto& hit : hits) {
            EXPECT_EQ(0, hit.Id());
            observed.push_back(hit.Position());
        }
    }
    EXPECT_EQ(expected, observed);
}

TEST(QGram_Index, index_hits_PUBLIC_API_empty_hits_result_when_query_seq_is_too_short)
{
    const PacBio::QGram::Index idx{6,
                                   "TCCAACTTAGGCATAAACCTGCATGCTACCTTGTCAGACCCACTCTGCACGAAGTAA"
                                   "ATATGGGATGCGTCCGACCTGGCTCCTGGCGTTCCACGCCGCCACGTGTTCGTTAAC"
                                   "TGTTGATTGGTGGCACATAAGTAATACCATGGTCCCTGAAATTCGGCTCAGTTACTT"
                                   "CGAGCGTAATGTCTCAAATGGCGTAGAACGGCAATGACTGTTTGACACTAGGTGGTG"
                                   "TTCAGTTCGGTAACGGAGAGTCTGTGCGGCATTCTTATTAATACATTTGAAACGCGC"
                                   "CCAACTGACGCTAGGCAAGTCAGTGCAGGCTCCCGTGTTAGGATAAGGGTAAACATA"
                                   "CAAGTCGATAGAAGATGGGTAGGGGCCTTCAATTCATCCAGCACTCTACGGTTCCTC"
                                   "CGAGAGCAAGTAGGGCACCCTGTAGTTCGAAGGGGAACTATTTCGTGGGGCGAGCCC"
                                   "ACACCGTCTCTTCTGCGGAAGACTTAACACGTTAGGGAGGTGGA"};

    EXPECT_TRUE(idx.Hits("").empty());
    EXPECT_TRUE(idx.Hits("ACG").empty());
}

#include <pbcopper/third-party/sparc/sparc.hpp>

#include <gtest/gtest.h>

/*
std::string SparcConsensus(const std::string& backbone, std::vector<Query>& queries, int32_t gap,
                           size_t k, int32_t MinCov, double threshold);
std::string SparcSimpleConsensus(const std::string& backbone, std::vector<Query>& queries,
                                 int32_t MinCov, double threshold);
 */

TEST(sparc, simple_okay)
{

    std::vector<Sparc::Query> queries;

    std::string backbone = "ATGAAAGAGAGAGAGAGAAGCTACAC";
    std::string mutate_1 = "ATGAAAGATAGAGAGAGAAGCTACAC";
    std::string mutate_2 = "ATGAAAGATAGAGAGAGAAGCTACAC";
    std::string mutate_3 = "ATGAAAGATAGAGAGAGAAGCTACAC";
    queries.emplace_back(Sparc::Query{
        mutate_1, backbone, 0,  // ref starts
        25,                     // ref end
    });

    queries.emplace_back(Sparc::Query{
        mutate_2, backbone, 0,  // ref starts
        25,                     // ref end
    });

    queries.emplace_back(Sparc::Query{
        mutate_3, backbone, 0,  // ref starts
        25,                     // ref end
    });

    const auto con = SparcSimpleConsensus(backbone, queries, 2, 0.35);
    EXPECT_EQ(con, "ATGAAAGATAGAGAGAGAAGCTACAC");
}

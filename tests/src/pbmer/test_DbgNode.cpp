#include <gtest/gtest.h>

#include <pbcopper/pbmer/Dbg.h>
#include <pbcopper/pbmer/DbgNode.h>
#include <pbcopper/pbmer/Parser.h>

TEST(Pbmer_DbgNode, can_iterate_over_one_a_dnabits)
{
    PacBio::Pbmer::DnaBit niby;
    PacBio::Pbmer::DbgNode eg{niby, 0};
    eg.SetEdges(2);
    int count = 0;
    for (const auto& i : eg) {
        (void)i;
        ++count;
    }
    EXPECT_EQ(count, 1);
}

TEST(Pbmer_DbgNode, can_iterate_over_one_b_dnabits)
{
    PacBio::Pbmer::DnaBit niby;
    PacBio::Pbmer::DbgNode eg{niby, 0};
    eg.SetEdges(1);
    int count = 0;
    for (const auto& i : eg) {
        (void)i;
        ++count;
    }
    EXPECT_EQ(count, 1);
}

TEST(Pbmer_DbgNode, can_iterate_over_one_c_dnabits)
{
    PacBio::Pbmer::DnaBit niby;
    niby.strand = 0;
    niby.msize = 4;
    niby.mer = 2;
    PacBio::Pbmer::DbgNode eg{niby, 0};
    eg.SetEdges(128);
    int count = 0;
    for (const auto& i : eg) {
        (void)i;
        ++count;
    }
    EXPECT_EQ(count, 1);
}

TEST(Pbmer_DbgNode, can_iterate_over_two_a_dnabits)
{
    PacBio::Pbmer::DnaBit niby;
    niby.strand = 0;
    niby.msize = 4;
    niby.mer = 2;

    PacBio::Pbmer::DbgNode eg{niby, 0};
    eg.SetEdges(129);
    int count = 0;
    for (const auto& i : eg) {
        (void)i;
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(Pbmer_DbgNode, can_iterate_over_two_b_dnabits)
{
    PacBio::Pbmer::DnaBit niby;
    PacBio::Pbmer::DbgNode eg{niby, 0};
    eg.SetEdges(5);
    int count = 0;
    for (const auto& i : eg) {
        (void)i;
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(Pbmer_DbgNode, can_iterate_over_three_a_dnabits)
{
    PacBio::Pbmer::DnaBit niby;
    PacBio::Pbmer::DbgNode eg{niby, 0};
    eg.SetEdges(7);
    int count = 0;
    for (const auto& i : eg) {
        (void)i;
        ++count;
    }
    EXPECT_EQ(count, 3);
}

TEST(Pbmer_DbgNode, can_iterate_over_two_node_a)
{
    const PacBio::Pbmer::Parser parser{21};
    const std::string td1{"AAAAAAGTCGCGGAACAAATCA"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{21, 1};

    dg.AddKmers(m1, 1);

    dg.BuildEdges();
    EXPECT_EQ(dg.ValidateEdges(), true);
}

TEST(Pbmer_DbgNode, can_iterate_over_two_node_b)
{
    const PacBio::Pbmer::Parser parser{21};
    const std::string td1{"CACAAAGTCGCGGAACAAATCA"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{21, 1};

    dg.AddKmers(m1, 1);

    dg.BuildEdges();
    EXPECT_EQ(dg.ValidateEdges(), true);
}

TEST(Pbmer_DbgNode, can_iterate_over_three_node_a)
{
    const PacBio::Pbmer::Parser parser{21};
    const std::string td1{"GCACAAAGTCGCGGAACAAATCA"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{21, 1};

    dg.AddKmers(m1, 1);

    dg.BuildEdges();
    EXPECT_EQ(dg.ValidateEdges(), true);
}

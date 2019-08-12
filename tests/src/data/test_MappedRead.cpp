#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/data/MappedRead.h>

// clang-format off

namespace MappedReadTests {

using Cigar = PacBio::Data::Cigar;
using Frames = PacBio::Data::Frames;
using GapBehavior = PacBio::Data::GapBehavior;
using MappedRead = PacBio::Data::MappedRead;
using Orientation = PacBio::Data::Orientation;
using QualityValues = PacBio::Data::QualityValues;
using Read = PacBio::Data::Read;
using SoftClipBehavior = PacBio::Data::SoftClipBehavior;
using Strand = PacBio::Data::Strand;

const PacBio::Data::SNR baseSNR{0.0, 0.0, 0.0, 0.0};

// generic result holder for various requested states
template <typename T>
struct Result
{
public:
    Result(std::initializer_list<T> init) : d_(init) { assert(12 == init.size()); }

    T ForwardGenomic() const { return d_.at(0); }
    T ForwardNative() const { return d_.at(1); }
    T ForwardGenomicAligned() const { return d_.at(2); }
    T ForwardNativeAligned() const { return d_.at(3); }
    T ForwardGenomicAlignedClipped() const { return d_.at(4); }
    T ForwardNativeAlignedClipped() const { return d_.at(5); }
    T ReverseGenomic() const { return d_.at(6); }
    T ReverseNative() const { return d_.at(7); }
    T ReverseGenomicAligned() const { return d_.at(8); }
    T ReverseNativeAligned() const { return d_.at(9); }
    T ReverseGenomicAlignedClipped() const { return d_.at(10); }
    T ReverseNativeAlignedClipped() const { return d_.at(11); }

private:
    std::vector<T> d_;
};

template<typename DataType, typename MakeReadType, typename FetchDataType>
void CheckAlignedResults(const DataType& input,
                         const Cigar& cigar,
                         const Result<DataType>& e,
                         const MakeReadType& makeRead,
                         const FetchDataType& fetchData)
{
    {   // map to forward strand
        const MappedRead mr = makeRead(input, cigar, Strand::FORWARD);
        EXPECT_EQ(e.ForwardGenomic(),               fetchData(mr, Orientation::GENOMIC, GapBehavior::IGNORE, SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ForwardNative(),                fetchData(mr, Orientation::NATIVE,  GapBehavior::IGNORE, SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ForwardGenomicAligned(),        fetchData(mr, Orientation::GENOMIC, GapBehavior::SHOW,   SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ForwardNativeAligned(),         fetchData(mr, Orientation::NATIVE,  GapBehavior::SHOW,   SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ForwardGenomicAlignedClipped(), fetchData(mr, Orientation::GENOMIC, GapBehavior::SHOW,   SoftClipBehavior::REMOVE));
        EXPECT_EQ(e.ForwardNativeAlignedClipped(),  fetchData(mr, Orientation::NATIVE,  GapBehavior::SHOW,   SoftClipBehavior::REMOVE));
    }
    {   // map to reverse strand
        const MappedRead mr = makeRead(input, cigar, Strand::REVERSE);
        EXPECT_EQ(e.ReverseGenomic(),               fetchData(mr, Orientation::GENOMIC, GapBehavior::IGNORE, SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ReverseNative(),                fetchData(mr, Orientation::NATIVE,  GapBehavior::IGNORE, SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ReverseGenomicAligned(),        fetchData(mr, Orientation::GENOMIC, GapBehavior::SHOW,   SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ReverseNativeAligned(),         fetchData(mr, Orientation::NATIVE,  GapBehavior::SHOW,   SoftClipBehavior::KEEP));
        EXPECT_EQ(e.ReverseGenomicAlignedClipped(), fetchData(mr, Orientation::GENOMIC, GapBehavior::SHOW,   SoftClipBehavior::REMOVE));
        EXPECT_EQ(e.ReverseNativeAlignedClipped(),  fetchData(mr, Orientation::NATIVE,  GapBehavior::SHOW,   SoftClipBehavior::REMOVE));
    }
}

void CheckAlignedSequence(const std::string& cigar,
                          const std::string& input,
                          const Result<std::string>& expected)
{
    // make mapped record w/ SEQ (native orientation)
    auto makeRead = [](std::string newSeq,
                       const Cigar& newCigar,
                       const Strand newStrand)
    {
        Read r{"", newSeq, QualityValues{}, baseSNR};
        MappedRead mr{r};
        mr.Cigar = newCigar;
        mr.Strand = newStrand;
        return mr;
    };

    auto fetchSeq = [](const MappedRead& mr,
                       const Orientation orientation,
                       const GapBehavior gapBehavior,
                       const SoftClipBehavior softClipBehavior)
    {
        return mr.AlignedSequence(orientation, gapBehavior, softClipBehavior);
    };

    CheckAlignedResults(input, Cigar{cigar}, expected, makeRead, fetchSeq);
}

void CheckAlignedQualities(const std::string& cigar,
                           const QualityValues& input,
                           const Result<QualityValues>& expected)
{
    // make mapped record w/ QUAL (native orientation)
    auto makeRead = [](QualityValues newQuals,
                       const Cigar& newCigar,
                       const Strand newStrand)
    {
        Read r{"", "", newQuals, baseSNR};
        MappedRead mr{r};
        mr.Cigar = newCigar;
        mr.Strand = newStrand;
        return mr;
    };

    auto fetchQual = [](const MappedRead& mr,
                        const Orientation orientation,
                        const GapBehavior gapBehavior,
                        const SoftClipBehavior softClipBehavior)
    {
        return mr.AlignedQualities(orientation, gapBehavior, softClipBehavior);
    };

    CheckAlignedResults(input, Cigar{cigar}, expected, makeRead, fetchQual);
}

void CheckAlignedFrames(const std::string& cigar,
                        const Frames& input,
                        const Result<Frames>& expected)
{
    // make mapped record w/ IPD & PulseWidth (native orientation)
    auto makeRead = [](const Frames& newFrames,
                       const Cigar& newCigar,
                       const Strand newStrand)
    {
        Read r{"", "", QualityValues{}, baseSNR};
        r.IPD = newFrames;
        r.PulseWidth = newFrames;
        MappedRead mr{r};
        mr.Cigar = newCigar;
        mr.Strand = newStrand;
        return mr;
    };

    auto fetchIPD = [](const MappedRead& mr,
                       const Orientation orientation,
                       const GapBehavior gapBehavior,
                       const SoftClipBehavior softClipBehavior)
    {
        return mr.AlignedIPD(orientation, gapBehavior, softClipBehavior);
    };

    auto fetchPulseWidth = [](const MappedRead& mr,
                              const Orientation orientation,
                              const GapBehavior gapBehavior,
                              const SoftClipBehavior softClipBehavior)
    {
        return mr.AlignedPulseWidth(orientation, gapBehavior, softClipBehavior);
    };

    CheckAlignedResults(input, Cigar{cigar}, expected, makeRead, fetchIPD);
    CheckAlignedResults(input, Cigar{cigar}, expected, makeRead, fetchPulseWidth);
}

}  // namespace MappedReadTests

TEST(Data_MappedRead, unmapped_strand_returns_input_sequence)
{
    const std::string seq{"GATTACA"};

    PacBio::Data::Read r{"", seq, {}, MappedReadTests::baseSNR};
    PacBio::Data::MappedRead mr{r};
    mr.Cigar = {"7="};
    mr.Strand = PacBio::Data::Strand::UNMAPPED;
    EXPECT_EQ(seq, mr.AlignedSequence());
}

TEST(Data_MappedRead, empty_cigar_returns_input_sequence)
{
    const std::string seq{"GATTACA"};

    PacBio::Data::Read r{"", seq, {}, MappedReadTests::baseSNR};
    PacBio::Data::MappedRead mr{r};
    mr.Strand = PacBio::Data::Strand::FORWARD;
    EXPECT_EQ(seq, mr.AlignedSequence());
}

TEST(Data_MappedRead, can_fetch_aligned_sequence)
{
    {
        SCOPED_TRACE("CIGAR: 4=3D4=");
        MappedReadTests::CheckAlignedSequence(
            "4=3D4=",           // CIGAR
            "AACCGTTA",         // native input
            {
                "AACCGTTA",     // forward strand, genomic
                "AACCGTTA",     // forward strand, native
                "AACC---GTTA",  // forward strand, genomic, aligned
                "AACC---GTTA",  // forward strand, native,  aligned
                "AACC---GTTA",  // forward strand, genomic, aligned + clipped
                "AACC---GTTA",  // forward strand, native,  aligned + clipped
                "TAACGGTT",     // reverse strand, genomic
                "AACCGTTA",     // reverse strand, native
                "TAAC---GGTT",  // reverse strand, genomic, aligned
                "AACC---GTTA",  // reverse strand, native,  aligned
                "TAAC---GGTT",  // reverse strand, genomic, aligned + clipped
                "AACC---GTTA"   // reverse strand, native,  aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 4=1D2I2D4=");
        MappedReadTests::CheckAlignedSequence(
            "4=1D2I2D4=",           // CIGAR
            "ATCCTAGGTT",           // native input
            {
                "ATCCTAGGTT",       // forward strand, genomic
                "ATCCTAGGTT",       // forward strand, native
                "ATCC-TA--GGTT",    // forward strand, genomic, aligned
                "ATCC-TA--GGTT",    // forward strand, native,  aligned
                "ATCC-TA--GGTT",    // forward strand, genomic, aligned + clipped
                "ATCC-TA--GGTT",    // forward strand, native,  aligned + clipped
                "AACCTAGGAT",       // reverse strand, genomic
                "ATCCTAGGTT",       // reverse strand, native
                "AACC-TA--GGAT",    // reverse strand, genomic, aligned
                "ATCC--TA-GGTT",    // reverse strand, native,  aligned
                "AACC-TA--GGAT",    // reverse strand, genomic, aligned + clipped
                "ATCC--TA-GGTT"     // reverse strand, native,  aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 4=1D2P2I2P2D4=");
        MappedReadTests::CheckAlignedSequence(
            "4=1D2P2I2P2D4=",           // CIGAR
            "ATCCTAGGTT",               // native input
            {
                "ATCCTAGGTT",           // forward strand, genomic
                "ATCCTAGGTT",           // forward strand, native
                "ATCC-**TA**--GGTT",    // forward strand, genomic, aligned
                "ATCC-**TA**--GGTT",    // forward strand, native,  aligned
                "ATCC-**TA**--GGTT",    // forward strand, genomic, aligned + clipped
                "ATCC-**TA**--GGTT",    // forward strand, native,  aligned + clipped
                "AACCTAGGAT",           // reverse strand, genomic
                "ATCCTAGGTT",           // reverse strand, native
                "AACC-**TA**--GGAT",    // reverse strand, genomic, aligned
                "ATCC--**TA**-GGTT",    // reverse strand, native,  aligned
                "AACC-**TA**--GGAT",    // reverse strand, genomic, aligned + clipped
                "ATCC--**TA**-GGTT"     // reverse strand, native,  aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2S4=3D4=3S");
        MappedReadTests::CheckAlignedSequence(
            "2S4=3D4=3S",               // CIGAR
            "TTAACCGTTACCG",            // native input
            {
                "TTAACCGTTACCG",        // forward strand, genomic
                "TTAACCGTTACCG",        // forward strand, native
                "TTAACC---GTTACCG",     // forward strand, genomic, aligned
                "TTAACC---GTTACCG",     // forward strand, native,  aligned
                "AACC---GTTA",          // forward strand, genomic, aligned + clipped
                "AACC---GTTA",          // forward strand, native,  aligned + clipped
                "CGGTAACGGTTAA",        // reverse strand, genomic
                "TTAACCGTTACCG",        // reverse strand, native
                "CGGTAA---CGGTTAA",     // reverse strand, genomic, aligned
                "TTAACCG---TTACCG",     // reverse strand, native,  aligned
                "GTAA---CGGT",          // reverse strand, genomic, aligned + clipped
                "ACCG---TTAC"           // reverse strand, native,  aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2H4=3D4=3H");
        MappedReadTests::CheckAlignedSequence(
            "2H4=3D4=3H",       // CIGAR
            "AACCGTTA",         // native input
            {
                "AACCGTTA",     // forward strand, genomic
                "AACCGTTA",     // forward strand, native
                "AACC---GTTA",  // forward strand, genomic, aligned
                "AACC---GTTA",  // forward strand, native,  aligned
                "AACC---GTTA",  // forward strand, genomic, aligned + clipped
                "AACC---GTTA",  // forward strand, native,  aligned + clipped
                "TAACGGTT",     // reverse strand, genomic
                "AACCGTTA",     // reverse strand, native
                "TAAC---GGTT",  // reverse strand, genomic, aligned
                "AACC---GTTA",  // reverse strand, native,  aligned
                "TAAC---GGTT",  // reverse strand, genomic, aligned + clipped
                "AACC---GTTA"   // reverse strand, native,  aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2H2S4=3D4=3S3H");
        MappedReadTests::CheckAlignedSequence(
            "2H2S4=3D4=3S3H",           // CIGAR
            "TTAACCGTTACCG",            // native input
            {
                "TTAACCGTTACCG",        // forward strand, genomic
                "TTAACCGTTACCG",        // forward strand, native
                "TTAACC---GTTACCG",     // forward strand, genomic, aligned
                "TTAACC---GTTACCG",     // forward strand, native,  aligned
                "AACC---GTTA",          // forward strand, genomic, aligned + clipped
                "AACC---GTTA",          // forward strand, native,  aligned + clipped
                "CGGTAACGGTTAA",        // reverse strand, genomic
                "TTAACCGTTACCG",        // reverse strand, native
                "CGGTAA---CGGTTAA",     // reverse strand, genomic, aligned
                "TTAACCG---TTACCG",     // reverse strand, native,  aligned
                "GTAA---CGGT",          // reverse strand, genomic, aligned + clipped
                "ACCG---TTAC"           // reverse strand, native,  aligned + clipped
            }
        );
    }
}

TEST(Data_MappedRead, unmapped_strand_returns_input_qualities)
{
    const PacBio::Data::QualityValues quals{"@@@@@@@"};

    PacBio::Data::Read r{"", "", quals, MappedReadTests::baseSNR};
    PacBio::Data::MappedRead mr{r};
    mr.Cigar = {"7="};
    mr.Strand = PacBio::Data::Strand::UNMAPPED;
    EXPECT_EQ(quals, mr.AlignedQualities());
}

TEST(Data_MappedRead, empty_cigar_returns_input_qualities)
{
    const PacBio::Data::QualityValues quals{"@@@@@@@"};

    PacBio::Data::Read r{"", "", quals, MappedReadTests::baseSNR};
    PacBio::Data::MappedRead mr{r};
    mr.Strand = PacBio::Data::Strand::FORWARD;
    EXPECT_EQ(quals, mr.AlignedQualities());
}

TEST(Data_MappedRead, can_fetch_aligned_qualities)
{
    using QualityValues = PacBio::Data::QualityValues;

    {
        SCOPED_TRACE("CIGAR: 4=3D4=");
        MappedReadTests::CheckAlignedQualities(
            "4=3D4=",                          // CIGAR
            QualityValues{"?]?]?]?@"},         // native input
            {
                QualityValues{"?]?]?]?@"},     // forward strand, genomic
                QualityValues{"?]?]?]?@"},     // forward strand, native
                QualityValues{"?]?]!!!?]?@"},  // forward strand, genomic, aligned
                QualityValues{"?]?]!!!?]?@"},  // forward strand, native,  aligned
                QualityValues{"?]?]!!!?]?@"},  // forward strand, genomic, aligned + clipped
                QualityValues{"?]?]!!!?]?@"},  // forward strand, native,  aligned + clipped
                QualityValues{"@?]?]?]?"},     // reverse strand, genomic
                QualityValues{"?]?]?]?@"},     // reverse strand, native
                QualityValues{"@?]?!!!]?]?"},  // reverse strand, genomic, aligned
                QualityValues{"?]?]!!!?]?@"},  // reverse strand, native,  aligned
                QualityValues{"@?]?!!!]?]?"},  // reverse strand, genomic, aligned + clipped
                QualityValues{"?]?]!!!?]?@"}   // reverse strand, native, aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 4=1D2I2D4=");
        MappedReadTests::CheckAlignedQualities(
            "4=1D2I2D4=",                          // CIGAR
            QualityValues{"?]?]87?]?@"},           // native input
            {
                QualityValues{"?]?]87?]?@"},       // forward strand, genomic
                QualityValues{"?]?]87?]?@"},       // forward strand, native
                QualityValues{"?]?]!87!!?]?@"},    // forward strand, genomic, aligned
                QualityValues{"?]?]!87!!?]?@"},    // forward strand, native,  aligned
                QualityValues{"?]?]!87!!?]?@"},    // forward strand, genomic, aligned + clipped
                QualityValues{"?]?]!87!!?]?@"},    // forward strand, native,  aligned + clipped
                QualityValues{"@?]?78]?]?"},       // reverse strand, genomic
                QualityValues{"?]?]87?]?@"},       // reverse strand, native
                QualityValues{"@?]?!78!!]?]?"},    // reverse strand, genomic, aligned
                QualityValues{"?]?]!!87!?]?@"},    // reverse strand, native,  aligned
                QualityValues{"@?]?!78!!]?]?"},    // reverse strand, genomic, aligned + clipped
                QualityValues{"?]?]!!87!?]?@"}     // reverse strand, native,  aligned + clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 4=1D2P2I2P2D4=");
        MappedReadTests::CheckAlignedQualities(
            "4=1D2P2I2P2D4=",                      // CIGAR
            QualityValues{"?]?]87?]?@"},           // native input
        {
            QualityValues{"?]?]87?]?@"},           // forward strand, genomic
            QualityValues{"?]?]87?]?@"},           // forward strand, native
            QualityValues{"?]?]!!!87!!!!?]?@"},    // forward strand, genomic, aligned
            QualityValues{"?]?]!!!87!!!!?]?@"},    // forward strand, native,  aligned
            QualityValues{"?]?]!!!87!!!!?]?@"},    // forward strand, genomic, aligned + clipped
            QualityValues{"?]?]!!!87!!!!?]?@"},    // forward strand, native,  aligned + clipped
            QualityValues{"@?]?78]?]?"},           // reverse strand, genomic
            QualityValues{"?]?]87?]?@"},           // reverse strand, native
            QualityValues{"@?]?!!!78!!!!]?]?"},    // reverse strand, genomic, aligned
            QualityValues{"?]?]!!!!87!!!?]?@"},    // reverse strand, native,  aligned
            QualityValues{"@?]?!!!78!!!!]?]?"},    // reverse strand, genomic, aligned + clipped
            QualityValues{"?]?]!!!!87!!!?]?@"}     // reverse strand, native,  aligned + clipped
        }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 3S4=3D4=3S");
        MappedReadTests::CheckAlignedQualities(
            "3S4=3D4=3S",                              // CIGAR
            QualityValues{"vvv?]?]?]?@xxx"},           // native input
            {
                QualityValues{"vvv?]?]?]?@xxx"},       // forward strand, genomic
                QualityValues{"vvv?]?]?]?@xxx"},       // forward strand, native
                QualityValues{"vvv?]?]!!!?]?@xxx"},    // forward strand, genomic, aligned
                QualityValues{"vvv?]?]!!!?]?@xxx"},    // forward strand, native, aligned
                QualityValues{"?]?]!!!?]?@"},          // forward strand, genomic, aligned, clipped
                QualityValues{"?]?]!!!?]?@"},          // forward strand, native, aligned, clipped
                QualityValues{"xxx@?]?]?]?vvv"},       // reverse strand, genomic
                QualityValues{"vvv?]?]?]?@xxx"},       // reverse strand, native
                QualityValues{"xxx@?]?!!!]?]?vvv"},    // reverse strand, genomic, aligned
                QualityValues{"vvv?]?]!!!?]?@xxx"},    // reverse strand, native, aligned
                QualityValues{"@?]?!!!]?]?"},          // reverse strand, genomic, aligned, clipped
                QualityValues{"?]?]!!!?]?@"}           // reverse strand, native, aligned, clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2H4=3D4=3H");
        MappedReadTests::CheckAlignedQualities(
            "2H4=3D4=3H",                      // CIGAR
            QualityValues{"?]?]?]?@"},         // native input
            {
                QualityValues{"?]?]?]?@"},     // forward strand, genomic
                QualityValues{"?]?]?]?@"},     // forward strand, native
                QualityValues{"?]?]!!!?]?@"},  // forward strand, genomic, aligned
                QualityValues{"?]?]!!!?]?@"},  // forward strand, native, aligned
                QualityValues{"?]?]!!!?]?@"},  // forward strand, genomic, aligned, clipped
                QualityValues{"?]?]!!!?]?@"},  // forward strand, native, aligned, clipped
                QualityValues{"@?]?]?]?"},     // reverse strand, genomic
                QualityValues{"?]?]?]?@"},     // reverse strand, native
                QualityValues{"@?]?!!!]?]?"},  // reverse strand, genomic, aligned
                QualityValues{"?]?]!!!?]?@"},  // reverse strand, native, aligned
                QualityValues{"@?]?!!!]?]?"},  // reverse strand, genomic, aligned, clipped
                QualityValues{"?]?]!!!?]?@"}   // reverse strand, native, aligned, clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2H3S4=3D4=3S3H");
        MappedReadTests::CheckAlignedQualities(
            "2H3S4=3D4=3S3H",                          // CIGAR
            QualityValues{"vvv?]?]?]?@xxx"},           // native input
            {
                QualityValues{"vvv?]?]?]?@xxx"},       // forward strand, genomic
                QualityValues{"vvv?]?]?]?@xxx"},       // forward strand, native
                QualityValues{"vvv?]?]!!!?]?@xxx"},    // forward strand, genomic, aligned
                QualityValues{"vvv?]?]!!!?]?@xxx"},    // forward strand, native, aligned
                QualityValues{"?]?]!!!?]?@"},          // forward strand, genomic, aligned, clipped
                QualityValues{"?]?]!!!?]?@"},          // forward strand, native, aligned, clipped
                QualityValues{"xxx@?]?]?]?vvv"},       // reverse strand, genomic
                QualityValues{"vvv?]?]?]?@xxx"},       // reverse strand, native
                QualityValues{"xxx@?]?!!!]?]?vvv"},    // reverse strand, genomic, aligned
                QualityValues{"vvv?]?]!!!?]?@xxx"},    // reverse strand, native, aligned
                QualityValues{"@?]?!!!]?]?"},          // reverse strand, genomic, aligned, clipped
                QualityValues{"?]?]!!!?]?@"}           // reverse strand, native, aligned, clipped
            }
        );
    }
}


TEST(Data_MappedRead, unmapped_strand_returns_input_frames)
{
    const PacBio::Data::Frames frames {10, 20, 10, 20, 10, 20, 10};

    PacBio::Data::Read r{"", "", {}, MappedReadTests::baseSNR};
    r.IPD = frames;
    r.PulseWidth = frames;
    PacBio::Data::MappedRead mr{r};
    mr.Cigar = {"7="};
    mr.Strand = PacBio::Data::Strand::UNMAPPED;

    EXPECT_EQ(frames, mr.AlignedIPD());
    EXPECT_EQ(frames, mr.AlignedPulseWidth());
}

TEST(Data_MappedRead, empty_cigar_returns_input_frames)
{
    const PacBio::Data::Frames frames {10, 20, 10, 20, 10, 20, 10};

    PacBio::Data::Read r{"", "", {}, MappedReadTests::baseSNR};
    r.IPD = frames;
    r.PulseWidth = frames;
    PacBio::Data::MappedRead mr{r};
    mr.Strand = PacBio::Data::Strand::FORWARD;

    EXPECT_EQ(frames, mr.AlignedIPD());
    EXPECT_EQ(frames, mr.AlignedPulseWidth());
}

TEST(Data_MappedRead, can_fetch_aligned_frames)
{
    {
        SCOPED_TRACE("CIGAR: 4=3D4=");
        MappedReadTests::CheckAlignedFrames(
            "4=3D4=",                                           // CIGAR
            { 10, 20, 10, 20, 10, 20, 10, 30 },                 // native input
            {
                { 10, 20, 10, 20, 10, 20, 10, 30 },             // forward strand, genomic
                { 10, 20, 10, 20, 10, 20, 10, 30 },             // forward strand, native
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned, clipped
                { 30, 10, 20, 10, 20, 10, 20, 10 },             // reverse strand, genomic
                { 10, 20, 10, 20, 10, 20, 10, 30 },             // reverse strand, native
                { 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // reverse strand, native, aligned
                { 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 }     // reverse strand, native, aligned, clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 4=1D2I2D4=");
        MappedReadTests::CheckAlignedFrames(
            "4=1D2I2D4=",                                               // CIGAR
            { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },                 // native input
            {
                { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },             // forward strand, genomic
                { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },             // forward strand, native
                { 10, 20, 10, 20, 0, 80, 70, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned
                { 10, 20, 10, 20, 0, 80, 70, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned
                { 10, 20, 10, 20, 0, 80, 70, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 80, 70, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned, clipped
                { 30, 10, 20, 10, 70, 80, 20, 10, 20, 10 },             // reverse strand, genomic
                { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },             // reverse strand, native
                { 30, 10, 20, 10, 0, 70, 80, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned
                { 10, 20, 10, 20, 0, 0, 80, 70, 0, 10, 20, 10, 30 },    // reverse strand, native, aligned
                { 30, 10, 20, 10, 0, 70, 80, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 80, 70, 0, 10, 20, 10, 30 }     // reverse strand, native, aligned, clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 4=1D2P2I2P2D4=");
        MappedReadTests::CheckAlignedFrames(
            "4=1D2P2I2P2D4=",                                                   // CIGAR
            { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },                         // native input
        {
            { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },                         // forward strand, genomic
            { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },                         // forward strand, native
            { 10, 20, 10, 20, 0, 0, 0, 80, 70, 0, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned
            { 10, 20, 10, 20, 0, 0, 0, 80, 70, 0, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned
            { 10, 20, 10, 20, 0, 0, 0, 80, 70, 0, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned, clipped
            { 10, 20, 10, 20, 0, 0, 0, 80, 70, 0, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned, clipped
            { 30, 10, 20, 10, 70, 80, 20, 10, 20, 10 },                         // reverse strand, genomic
            { 10, 20, 10, 20, 80, 70, 10, 20, 10, 30 },                         // reverse strand, native
            { 30, 10, 20, 10, 0, 0, 0, 70, 80, 0, 0, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned
            { 10, 20, 10, 20, 0, 0, 0, 0, 80, 70, 0, 0, 0, 10, 20, 10, 30 },    // reverse strand, native, aligned
            { 30, 10, 20, 10, 0, 0, 0, 70, 80, 0, 0, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned, clipped
            { 10, 20, 10, 20, 0, 0, 0, 0, 80, 70, 0, 0, 0, 10, 20, 10, 30 }     // reverse strand, native, aligned, clipped
        }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 3S4=3D4=3S");
        MappedReadTests::CheckAlignedFrames(
            "3S4=3D4=3S",                                                               // CIGAR
            { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },                 // native input
            {
                { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },             // forward strand, genomic
                { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },             // forward strand, native
                { 40, 40, 40, 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30, 50, 50, 50 },    // forward strand, genomic, aligned
                { 40, 40, 40, 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30, 50, 50, 50 },    // forward strand, native, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },                            // forward strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },                            // forward strand, native, aligned, clipped
                { 50, 50, 50, 30, 10, 20, 10, 20, 10, 20, 10, 40, 40, 40 },             // reverse strand, genomic
                { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },             // reverse strand, native
                { 50, 50, 50, 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10, 40, 40, 40 },    // reverse strand, genomic, aligned
                { 40, 40, 40, 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30, 50, 50, 50 },    // reverse strand, native, aligned
                { 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10 },                            // reverse strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 }                             // reverse strand, native, aligned, clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2H4=3D4=3H");
        MappedReadTests::CheckAlignedFrames(
            "2H4=3D4=3H",                                       // CIGAR
            { 10, 20, 10, 20, 10, 20, 10, 30 },                 // native input
            {
                { 10, 20, 10, 20, 10, 20, 10, 30 },             // forward strand, genomic
                { 10, 20, 10, 20, 10, 20, 10, 30 },             // forward strand, native
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // forward strand, native, aligned, clipped
                { 30, 10, 20, 10, 20, 10, 20, 10 },             // reverse strand, genomic
                { 10, 20, 10, 20, 10, 20, 10, 30 },             // reverse strand, native
                { 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },    // reverse strand, native, aligned
                { 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10 },    // reverse strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 }     // reverse strand, native, aligned, clipped
            }
        );
    }
    {
        SCOPED_TRACE("CIGAR: 2H3S4=3D4=3S3H");
        MappedReadTests::CheckAlignedFrames(
            "2H3S4=3D4=3S3H",                                                           // CIGAR
            { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },                 // native input
            {
                { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },             // forward strand, genomic
                { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },             // forward strand, native
                { 40, 40, 40, 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30, 50, 50, 50 },    // forward strand, genomic, aligned
                { 40, 40, 40, 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30, 50, 50, 50 },    // forward strand, native, aligned
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },                            // forward strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 },                            // forward strand, native, aligned, clipped
                { 50, 50, 50, 30, 10, 20, 10, 20, 10, 20, 10, 40, 40, 40 },             // reverse strand, genomic
                { 40, 40, 40, 10, 20, 10, 20, 10, 20, 10, 30, 50, 50, 50 },             // reverse strand, native
                { 50, 50, 50, 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10, 40, 40, 40 },    // reverse strand, genomic, aligned
                { 40, 40, 40, 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30, 50, 50, 50 },    // reverse strand, native, aligned
                { 30, 10, 20, 10, 0, 0, 0, 20, 10, 20, 10 },                            // reverse strand, genomic, aligned, clipped
                { 10, 20, 10, 20, 0, 0, 0, 10, 20, 10, 30 }                             // reverse strand, native, aligned, clipped
            }
        );
    }
}

// clang-format on

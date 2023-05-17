#include <pbcopper/data/Read.h>

#include <algorithm>

#include <gtest/gtest.h>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/MappedRead.h>
#include <pbcopper/data/internal/ClippingImpl.h>

using namespace PacBio::Data;

TEST(Data_Read, ClipRead)
{
    const std::string seq{"AACCGTTAGC"};
    const QualityValues quals = QualityValues::FromFastq("0123456789");
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 500;
    const Position qEnd = 510;
    const std::vector<std::uint16_t> pw{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const std::vector<std::uint16_t> ipd{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    // ClipToQuery(read, 502, 509);
    const std::size_t clipStart = 502;
    const std::size_t clipEnd = 509;
    const ClipResult clipResult{2, 502, 509};

    Read read{"name/0/500_510", seq, quals, snr, qStart, qEnd, pw, ipd};
    internal::ClipRead(read, clipResult, clipStart, clipEnd);

    const std::string expectedSeq{"CCGTTAG"};
    const QualityValues expectedQuals = QualityValues::FromFastq("2345678");
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    const Position expectedQStart = 502;
    const Position expectedQEnd = 509;
    const std::vector<std::uint16_t> expectedPw{30, 40, 50, 60, 70, 80, 90};
    const std::vector<std::uint16_t> expectedIpd{30, 40, 50, 60, 70, 80, 90};

    EXPECT_EQ("name/0/500_510", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(read.PulseWidth);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(read.IPD);
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
}

TEST(Data_Read, ClipMappedRead)
{
    const std::string seq{"AACCGTTAGC"};
    const QualityValues quals = QualityValues::FromFastq("0123456789");
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 500;
    const Position qEnd = 510;
    const std::vector<std::uint16_t> pw{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const std::vector<std::uint16_t> ipd{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const Strand strand = Strand::FORWARD;
    const Position tStart = 100;
    const Position tEnd = 111;
    const Cigar cigar{"4=1D2I2D4="};
    const std::uint8_t mapQV = 80;

    // ClipToReference(read, 102, 107);
    const ClipResult clipResult{2, 502, 507, 102, Cigar{"2=1D2I2D"}};

    MappedRead read{Read{"name/0/500_510", seq, quals, snr, qStart, qEnd, pw, ipd},
                    strand,
                    tStart,
                    tEnd,
                    cigar,
                    mapQV};
    internal::ClipMappedRead(read, clipResult);

    const std::string expectedSeq{"CCGTT"};
    const QualityValues expectedQuals = QualityValues::FromFastq("23456");
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    const Position expectedQStart = 502;
    const Position expectedQEnd = 507;
    const std::vector<std::uint16_t> expectedPw{30, 40, 50, 60, 70};
    const std::vector<std::uint16_t> expectedIpd{30, 40, 50, 60, 70};
    const Strand expectedStrand = Strand::FORWARD;
    const Position expectedTStart = 102;
    const Position expectedTEnd = 107;
    const Cigar expectedCigar{"2=1D2I2D"};
    const std::uint8_t expectedMapQV = 80;

    EXPECT_EQ("name/0/500_510", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);
}

TEST(Data_Read, ClipToReferenceOutsideAlignedRegion)
{
    const std::string seq{"GATTACA"};
    const QualityValues quals{"ZZZZZZZ"};
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 500;
    const Position qEnd = 507;
    const std::vector<std::uint16_t> pw{10, 20, 30, 40, 50, 60, 70};
    const std::vector<std::uint16_t> ipd{10, 20, 30, 40, 50, 60, 70};
    const Strand strand = Strand::FORWARD;
    const Position tStart = 200;
    const Position tEnd = 207;
    const Cigar cigar{"7="};
    const std::uint8_t mapQV = 99;

    const std::string expectedSeq;
    const QualityValues expectedQuals{""};
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    const Position expectedQStart = -1;
    const Position expectedQEnd = -1;
    const std::vector<std::uint16_t> expectedPw{};
    const std::vector<std::uint16_t> expectedIpd{};
    const Strand expectedStrand = Strand::FORWARD;
    const Position expectedTStart = -1;
    const Position expectedTEnd = -1;
    const Cigar expectedCigar{""};
    const std::uint8_t expectedMapQV = 255;

    auto shouldClipToEmptyRead = [&](Position start, Position end) {
        MappedRead read{Read{"name/0/500_507", seq, quals, snr, qStart, qEnd, pw, ipd},
                        strand,
                        tStart,
                        tEnd,
                        cigar,
                        mapQV};

        ClipToReference(read, start, end, true);

        EXPECT_EQ("name/0/500_507", read.FullName());
        EXPECT_EQ(expectedSeq, read.Seq);
        EXPECT_EQ(expectedQuals, read.Qualities);
        EXPECT_EQ(expectedQStart, read.QueryStart);
        EXPECT_EQ(expectedQEnd, read.QueryEnd);
        EXPECT_EQ(expectedSnr, read.SignalToNoise);
        EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
        EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
        EXPECT_EQ(expectedStrand, read.Strand);
        EXPECT_EQ(expectedTStart, read.TemplateStart);
        EXPECT_EQ(expectedTEnd, read.TemplateEnd);
        EXPECT_EQ(expectedCigar, read.Cigar);
        EXPECT_EQ(expectedMapQV, read.MapQuality);
    };

    {
        SCOPED_TRACE("clip region is well before alignment start");
        shouldClipToEmptyRead(0, 50);
    }
    {
        SCOPED_TRACE("clip region ends at alignment start");
        shouldClipToEmptyRead(150, 200);
    }
    {
        SCOPED_TRACE("clip region starts at alignment end");
        shouldClipToEmptyRead(207, 250);
    }
    {
        SCOPED_TRACE("clip region starts well after alignment end");
        shouldClipToEmptyRead(500, 600);
    }
}

TEST(Data_Read, MultipleClipsToReference)
{
    const std::string seq(1200, 'A');
    const QualityValues quals{std::string(1200, 'Z')};
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 0;
    const Position qEnd = 1200;
    const std::vector<std::uint16_t> pw(1200, 20);
    const std::vector<std::uint16_t> ipd(1200, 20);
    const Strand strand = Strand::FORWARD;
    const Position tStart = 0;
    const Position tEnd = 1200;
    const Cigar cigar{"1200="};
    const std::uint8_t mapQV = 99;

    // intial read, aligned to reference: [0, 1200)
    MappedRead read{Read{"name/0/0_1200", seq, quals, snr, qStart, qEnd, pw, ipd},
                    strand,
                    tStart,
                    tEnd,
                    cigar,
                    mapQV};

    // clip to reference: [0, 1000) - shrinking from right
    ClipToReference(read, 0, 1000, true);

    std::string expectedSeq(1000, 'A');
    QualityValues expectedQuals{std::string(1000, 'Z')};
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    Position expectedQStart = 0;
    Position expectedQEnd = 1000;
    std::vector<std::uint16_t> expectedPw(1000, 20);
    std::vector<std::uint16_t> expectedIpd(1000, 20);
    const Strand expectedStrand = Strand::FORWARD;
    Position expectedTStart = 0;
    Position expectedTEnd = 1000;
    Cigar expectedCigar{"1000="};
    const std::uint8_t expectedMapQV = 99;

    EXPECT_EQ("name/0/0_1200", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [100, 1000) - shrinking from left
    ClipToReference(read, 100, 1000, true);

    expectedSeq = std::string(900, 'A');
    expectedQuals = QualityValues{std::string(900, 'Z')};
    expectedQStart = 100;
    expectedQEnd = 1000;
    expectedPw = std::vector<std::uint16_t>(900, 20);
    expectedIpd = std::vector<std::uint16_t>(900, 20);
    expectedTStart = 100;
    expectedTEnd = 1000;
    expectedCigar = Cigar{"900="};

    EXPECT_EQ("name/0/0_1200", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [200, 800) - shrinking both sides
    ClipToReference(read, 200, 800, true);

    expectedSeq = std::string(600, 'A');
    expectedQuals = QualityValues{std::string(600, 'Z')};
    expectedQStart = 200;
    expectedQEnd = 800;
    expectedPw = std::vector<std::uint16_t>(600, 20);
    expectedIpd = std::vector<std::uint16_t>(600, 20);
    expectedTStart = 200;
    expectedTEnd = 800;
    expectedCigar = Cigar{"600="};

    EXPECT_EQ("name/0/0_1200", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);
}

TEST(Data_Read, MultipleClipsToReference_WithLargeDeletion)
{
    const std::string seq(1000, 'A');
    const QualityValues quals{std::string(1000, 'Z')};
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 0;
    const Position qEnd = 1000;
    const std::vector<std::uint16_t> pw(1200, 20);
    const std::vector<std::uint16_t> ipd(1200, 20);
    const Strand strand = Strand::FORWARD;
    const Position tStart = 0;
    const Position tEnd = 1200;
    const Cigar cigar{"400=200D600="};
    const std::uint8_t mapQV = 99;

    // intial read, aligned to reference: [0, 1200)
    MappedRead read{Read{"name/0/0_1000", seq, quals, snr, qStart, qEnd, pw, ipd},
                    strand,
                    tStart,
                    tEnd,
                    cigar,
                    mapQV};

    // clip to reference: [0, 1000) - shrinking from right
    ClipToReference(read, 0, 1000, true);

    std::string expectedSeq(800, 'A');
    QualityValues expectedQuals{std::string(800, 'Z')};
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    Position expectedQStart = 0;
    Position expectedQEnd = 800;
    std::vector<std::uint16_t> expectedPw(800, 20);
    std::vector<std::uint16_t> expectedIpd(800, 20);
    const Strand expectedStrand = Strand::FORWARD;
    Position expectedTStart = 0;
    Position expectedTEnd = 1000;
    Cigar expectedCigar{"400=200D400="};
    const std::uint8_t expectedMapQV = 99;

    EXPECT_EQ("name/0/0_1000", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [100, 1000) - shrinking from left
    ClipToReference(read, 100, 1000, true);

    expectedSeq = std::string(700, 'A');
    expectedQuals = QualityValues{std::string(700, 'Z')};
    expectedQStart = 100;
    expectedQEnd = 800;
    expectedPw = std::vector<std::uint16_t>(700, 20);
    expectedIpd = std::vector<std::uint16_t>(700, 20);
    expectedTStart = 100;
    expectedTEnd = 1000;
    expectedCigar = Cigar{"300=200D400="};

    EXPECT_EQ("name/0/0_1000", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [200, 800) - shrinking both sides
    ClipToReference(read, 200, 800, true);

    expectedSeq = std::string(400, 'A');
    expectedQuals = QualityValues{std::string(400, 'Z')};
    expectedQStart = 200;
    expectedQEnd = 600;
    expectedPw = std::vector<std::uint16_t>(400, 20);
    expectedIpd = std::vector<std::uint16_t>(400, 20);
    expectedTStart = 200;
    expectedTEnd = 800;
    expectedCigar = Cigar{"200=200D200="};

    EXPECT_EQ("name/0/0_1000", read.FullName());
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth->begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);
}

TEST(Data_Read, BamRecordFunctions)
{
    const Position qStart = 500;
    const Position qEnd = 510;
    const std::string seq{"AACCGTTAGC"};
    const std::string quals{"?]?]?]?]?*"};
    const std::vector<std::uint8_t> frames = {10, 10, 20, 20, 30, 40, 40, 10, 30, 20};

    const std::string seq_rev{"GCTAACGGTT"};
    const std::string quals_rev{"*?]?]?]?]?"};
    const std::vector<std::uint8_t> frames_rev = {20, 30, 10, 40, 40, 30, 20, 20, 10, 10};

    const std::string s1_cigar{"10="};
    const std::string s2_cigar{"5=3D5="};
    const std::string s3_cigar{"4=1D2I2D4="};

    {
        // s1 - FORWARD
        const MappedRead s1{
            Read{"name/0/500_510", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::FORWARD, 100, Cigar::FromStdString(s1_cigar), 60};

        EXPECT_EQ(Strand::FORWARD, s1.AlignedStrand());
        EXPECT_EQ(qStart, s1.QueryStart);
        EXPECT_EQ(qEnd, s1.QueryEnd);
        EXPECT_EQ(500, s1.AlignedStart());
        EXPECT_EQ(510, s1.AlignedEnd());  // 500 + 10=
        EXPECT_EQ(100, s1.ReferenceStart());
        EXPECT_EQ(110, s1.ReferenceEnd());  // 100 + 10=
    }

    {
        // s1 - REVERSE
        const MappedRead s1_rev{
            Read{"name/0/500_510", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::REVERSE, 100, Cigar::FromStdString(s1_cigar), 60};

        EXPECT_EQ(Strand::REVERSE, s1_rev.AlignedStrand());
        EXPECT_EQ(qStart, s1_rev.QueryStart);
        EXPECT_EQ(qEnd, s1_rev.QueryEnd);
        EXPECT_EQ(500, s1_rev.AlignedStart());
        EXPECT_EQ(510, s1_rev.AlignedEnd());  // 500 + 10=
        EXPECT_EQ(100, s1_rev.ReferenceStart());
        EXPECT_EQ(110, s1_rev.ReferenceEnd());  // 100 + 10=
    }

    {
        // s2 - FORWARD
        const MappedRead s2{
            Read{"name/0/500_510", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::FORWARD, 100, Cigar::FromStdString(s2_cigar), 60};

        EXPECT_EQ(Strand::FORWARD, s2.AlignedStrand());
        EXPECT_EQ(qStart, s2.QueryStart);
        EXPECT_EQ(qEnd, s2.QueryEnd);
        EXPECT_EQ(500, s2.AlignedStart());
        EXPECT_EQ(510, s2.AlignedEnd());  // 500 + 10=
        EXPECT_EQ(100, s2.ReferenceStart());
        EXPECT_EQ(113, s2.ReferenceEnd());  // 100 + 10=
    }

    {
        // s2 - REVERSE
        const MappedRead s2_rev{
            Read{"name/0/500_510", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::REVERSE, 100, Cigar::FromStdString(s2_cigar), 60};

        EXPECT_EQ(Strand::REVERSE, s2_rev.AlignedStrand());
        EXPECT_EQ(qStart, s2_rev.QueryStart);
        EXPECT_EQ(qEnd, s2_rev.QueryEnd);
        EXPECT_EQ(500, s2_rev.AlignedStart());
        EXPECT_EQ(510, s2_rev.AlignedEnd());  // 500 + 10=
        EXPECT_EQ(100, s2_rev.ReferenceStart());
        EXPECT_EQ(113, s2_rev.ReferenceEnd());  // 100 + 10=
    }

    {
        // s3 - FORWARD
        const MappedRead s3{
            Read{"name/0/500_510", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::FORWARD, 100, Cigar::FromStdString(s3_cigar), 60};

        EXPECT_EQ(Strand::FORWARD, s3.AlignedStrand());
        EXPECT_EQ(qStart, s3.QueryStart);
        EXPECT_EQ(qEnd, s3.QueryEnd);
        EXPECT_EQ(500, s3.AlignedStart());
        EXPECT_EQ(510, s3.AlignedEnd());  // 500 + 10=
        EXPECT_EQ(100, s3.ReferenceStart());
        EXPECT_EQ(111, s3.ReferenceEnd());  // 100 + 10=
    }

    {
        // s3 - REVERSE
        const MappedRead s3_rev{
            Read{"name/0/500_510", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::REVERSE, 100, Cigar::FromStdString(s3_cigar), 60};

        EXPECT_EQ(Strand::REVERSE, s3_rev.AlignedStrand());
        EXPECT_EQ(qStart, s3_rev.QueryStart);
        EXPECT_EQ(qEnd, s3_rev.QueryEnd);
        EXPECT_EQ(500, s3_rev.AlignedStart());
        EXPECT_EQ(510, s3_rev.AlignedEnd());  // 500 + 10=
        EXPECT_EQ(100, s3_rev.ReferenceStart());
        EXPECT_EQ(111, s3_rev.ReferenceEnd());  // 100 + 10=
    }
}

TEST(Data_Read, BamRecordFunctions_Clip)
{
    const Position qStart = 500;
    const Position qEnd = 515;
    const std::string seq{"TTAACCGTTAGCAAA"};
    const std::string quals{"--?]?]?]?]?*+++"};
    const std::vector<std::uint8_t> frames = {40, 40, 10, 10, 20, 20, 30, 40,
                                              40, 10, 30, 20, 10, 10, 10};

    const std::string seq_rev{"TTTGCTAACGGTTAA"};
    const std::string quals_rev{"+++*?]?]?]?]?--"};
    const std::vector<std::uint8_t> frames_rev = {10, 10, 10, 20, 30, 10, 40, 40,
                                                  30, 20, 20, 10, 10, 40, 40};

    const std::string s1_cigar{"2S10=3S"};
    const std::string s2_cigar{"2S5=3D5=3S"};
    const std::string s3_cigar{"2S4=1D2I2D4=3S"};

    {
        // s1 - FORWARD
        const MappedRead s1{
            Read{"name/0/500_515", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::FORWARD, 100, Cigar::FromStdString(s1_cigar), 60};

        EXPECT_EQ(Strand::FORWARD, s1.AlignedStrand());
        EXPECT_EQ(qStart, s1.QueryStart);     // 500
        EXPECT_EQ(qEnd, s1.QueryEnd);         // QStart + seqLength
        EXPECT_EQ(502, s1.AlignedStart());    // QStart + 2S
        EXPECT_EQ(512, s1.AlignedEnd());      // AStart + 10=
        EXPECT_EQ(100, s1.ReferenceStart());  // 100
        EXPECT_EQ(110, s1.ReferenceEnd());    // RefStart + 10=
    }

    {
        // s1 - REVERSE
        const MappedRead s1_rev{
            Read{"name/0/500_515", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::REVERSE, 100, Cigar::FromStdString(s1_cigar), 60};

        EXPECT_EQ(Strand::REVERSE, s1_rev.AlignedStrand());
        EXPECT_EQ(qStart, s1_rev.QueryStart);     // 500
        EXPECT_EQ(qEnd, s1_rev.QueryEnd);         // QStart + seqLength
        EXPECT_EQ(503, s1_rev.AlignedStart());    // QStart + 3S
        EXPECT_EQ(513, s1_rev.AlignedEnd());      // AStart + 10=
        EXPECT_EQ(100, s1_rev.ReferenceStart());  // 100
        EXPECT_EQ(110, s1_rev.ReferenceEnd());    // RefStart + 10=
    }

    {
        // s2 - FORWARD
        const MappedRead s2{
            Read{"name/0/500_515", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::FORWARD, 100, Cigar::FromStdString(s2_cigar), 60};

        EXPECT_EQ(Strand::FORWARD, s2.AlignedStrand());
        EXPECT_EQ(qStart, s2.QueryStart);     // 500
        EXPECT_EQ(qEnd, s2.QueryEnd);         // QStart + seqLength
        EXPECT_EQ(502, s2.AlignedStart());    // QStart + 2S
        EXPECT_EQ(512, s2.AlignedEnd());      // AStart + 10=
        EXPECT_EQ(100, s2.ReferenceStart());  // 100
        EXPECT_EQ(113, s2.ReferenceEnd());    // RefStart + 10= + 3D
    }

    {
        // s2 - REVERSE
        const MappedRead s2_rev{
            Read{"name/0/500_515", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::REVERSE, 100, Cigar::FromStdString(s2_cigar), 60};

        EXPECT_EQ(Strand::REVERSE, s2_rev.AlignedStrand());
        EXPECT_EQ(qStart, s2_rev.QueryStart);     // 500
        EXPECT_EQ(qEnd, s2_rev.QueryEnd);         // QStart + seqLength
        EXPECT_EQ(503, s2_rev.AlignedStart());    // QStart + 3S
        EXPECT_EQ(513, s2_rev.AlignedEnd());      // AStart + 10=
        EXPECT_EQ(100, s2_rev.ReferenceStart());  // 100
        EXPECT_EQ(113, s2_rev.ReferenceEnd());    // RefStart + 10= + 3D
    }

    {
        // s3 - FORWARD
        const MappedRead s3{
            Read{"name/0/500_515", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::FORWARD, 100, Cigar::FromStdString(s3_cigar), 60};

        EXPECT_EQ(Strand::FORWARD, s3.AlignedStrand());
        EXPECT_EQ(qStart, s3.QueryStart);     // 500
        EXPECT_EQ(qEnd, s3.QueryEnd);         // QStart + seqLength
        EXPECT_EQ(502, s3.AlignedStart());    // QStart + 2S
        EXPECT_EQ(512, s3.AlignedEnd());      // AStart + 8= + 2I
        EXPECT_EQ(100, s3.ReferenceStart());  // 100
        EXPECT_EQ(111, s3.ReferenceEnd());    // RefStart + 8= + 3D
    }

    {
        // s3 - REVERSE
        const MappedRead s3_rev{
            Read{"name/0/500_515", seq, QualityValues{quals}, SNR{1, 2, 3, 4}, qStart, qEnd},
            Strand::REVERSE, 100, Cigar::FromStdString(s3_cigar), 60};

        EXPECT_EQ(Strand::REVERSE, s3_rev.AlignedStrand());
        EXPECT_EQ(qStart, s3_rev.QueryStart);     // 500
        EXPECT_EQ(qEnd, s3_rev.QueryEnd);         // QStart + seqLength
        EXPECT_EQ(503, s3_rev.AlignedStart());    // QStart + 3S
        EXPECT_EQ(513, s3_rev.AlignedEnd());      // AStart + 8= + 2I
        EXPECT_EQ(100, s3_rev.ReferenceStart());  // 100
        EXPECT_EQ(111, s3_rev.ReferenceEnd());    // RefStart + 8= + 3D
    }
}

// clang-format off
TEST(Data_Read, can_set_query_start_and_end_from_id)
{
    const std::int32_t holeNumber = 77;
    const PacBio::Data::Position qStart = 1000;
    const PacBio::Data::Position qEnd = 1010;
    const PacBio::Data::LocalContextFlags ctxtFlags =
        PacBio::Data::LocalContextFlags::ADAPTER_BEFORE |
        PacBio::Data::LocalContextFlags::ADAPTER_AFTER;
    const PacBio::Data::Accuracy acc = 0.95F;
    const PacBio::Data::SNR snr{0.4, 0.4, 0.4, 0.4};
    const std::string seq{"GGTTAACCAA"};
    const PacBio::Data::Frames pw{3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    const PacBio::Data::Frames ipd{3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    const std::string movie{"movie"};
    const std::string chemistry{"chemistry"};

    const PacBio::Data::Read read{
        PacBio::Data::ReadId{movie, holeNumber, {qStart, qEnd}},
        seq, pw, ipd, ctxtFlags, acc, snr, chemistry};
    EXPECT_EQ(qStart, read.QueryStart);
    EXPECT_EQ(qEnd, read.QueryEnd);
}

TEST(Data_ReadId, can_create_from_subread_name)
{
    const PacBio::Data::ReadId id{"m54001_160623_195125/553/3100_11230"};
    EXPECT_EQ("m54001_160623_195125", id.MovieName);
    EXPECT_EQ(553, id.HoleNumber);
    EXPECT_EQ(PacBio::Data::Interval(3100, 11230), *id.ZmwInterval);
}

TEST(Data_ReadId, can_create_from_ccs_read_name)
{
    const PacBio::Data::ReadId id{"m54001_160623_195125/553/ccs"};
    EXPECT_EQ("m54001_160623_195125", id.MovieName);
    EXPECT_EQ(553, id.HoleNumber);
    EXPECT_FALSE(id.ZmwInterval);
}

// clang-format on

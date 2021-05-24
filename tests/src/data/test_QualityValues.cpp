#include <pbcopper/data/QualityValues.h>

#include <limits>
#include <string>
#include <vector>

#include <gtest/gtest.h>

using QualityValue = PacBio::Data::QualityValue;
using QualityValues = PacBio::Data::QualityValues;

TEST(Data_QualityValue, constructs_expected_defaults)
{
    const QualityValue value;
    EXPECT_EQ(0, value);
    EXPECT_EQ('!', value.Fastq());
}

TEST(Data_QualityValue, can_be_constructed_from_integers)
{
    const QualityValue zero{0};
    const QualityValue thirtyThree{33};
    const QualityValue valid{42};
    const QualityValue max{93};
    const QualityValue tooHigh{94};
    const QualityValue wayTooHigh{std::numeric_limits<int8_t>::max()};

    EXPECT_EQ(0, zero);
    EXPECT_EQ(33, thirtyThree);
    EXPECT_EQ(42, valid);
    EXPECT_EQ(93, max);
    EXPECT_EQ(93, tooHigh);
    EXPECT_EQ(93, wayTooHigh);

    EXPECT_EQ('!', zero.Fastq());
    EXPECT_EQ('B', thirtyThree.Fastq());
    EXPECT_EQ('K', valid.Fastq());
    EXPECT_EQ('~', max.Fastq());
    EXPECT_EQ('~', tooHigh.Fastq());
    EXPECT_EQ('~', wayTooHigh.Fastq());
}

TEST(Data_QualityValue, can_be_constructed_from_fastq_chars)
{
    const QualityValue zero = QualityValue::FromFastq('!');
    const QualityValue thirtyThree = QualityValue::FromFastq('B');
    const QualityValue valid = QualityValue::FromFastq('K');
    const QualityValue max = QualityValue::FromFastq('~');

    EXPECT_EQ(0, zero);
    EXPECT_EQ(33, thirtyThree);
    EXPECT_EQ(42, valid);
    EXPECT_EQ(93, max);
}

TEST(Data_QualityValues, default_is_empty)
{
    const QualityValues qvs;
    EXPECT_TRUE(qvs.empty());
    EXPECT_EQ(std::string(), qvs.Fastq());
}

TEST(Data_QualityValues, can_be_constructed_from_integers)
{
    const std::vector<uint8_t> values{93, 93, 93, 42, 42, 33, 33, 0, 0};

    QualityValues qvs;
    for (auto qv : values) {
        qvs.push_back(qv);
    }

    EXPECT_EQ("~~~KKBB!!", qvs.Fastq());
}

TEST(Data_QualityValues, can_be_constructed_from_integer_vector)
{
    const std::vector<uint8_t> values{93, 93, 93, 42, 42, 33, 33, 0, 0};
    const QualityValues qvs{values};
    EXPECT_EQ("~~~KKBB!!", qvs.Fastq());
}

TEST(Data_QualityValues, can_be_constructed_from_quality_value_vector)
{
    const std::vector<QualityValue> values{93, 93, 93, 42, 42, 33, 33, 0, 0};
    const QualityValues qvs{values};
    EXPECT_EQ("~~~KKBB!!", qvs.Fastq());
}

TEST(Data_QualityValues, can_be_constructed_from_fastq_string)
{
    const std::string fastqString{"~~~KKBB!!"};
    const std::vector<uint8_t> expected{93, 93, 93, 42, 42, 33, 33, 0, 0};

    const QualityValues qvs = QualityValues::FromFastq(fastqString);
    EXPECT_EQ(fastqString.size(), qvs.size());

    ASSERT_EQ(expected.size(), qvs.size());
    for (size_t i = 0; i < fastqString.size(); ++i) {
        EXPECT_EQ(expected.at(i), qvs.at(i));
    }
}

// clang-format on

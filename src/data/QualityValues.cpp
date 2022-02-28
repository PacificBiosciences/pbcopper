#include <pbcopper/data/QualityValues.h>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <ostream>
#include <type_traits>

namespace PacBio {
namespace Data {

QualityValues::QualityValues(const std::string& fastqString) : std::vector<QualityValue>{}
{
    std::string fastqString_{std::move(fastqString)};
    boost::algorithm::trim(fastqString_);
    resize(fastqString_.size());
    std::transform(fastqString_.cbegin(), fastqString_.cend(), begin(), QualityValue::FromFastq);
}

QualityValues::QualityValues(std::vector<QualityValue> quals)
    : std::vector<QualityValue>{std::move(quals)}
{}

QualityValues::QualityValues(const std::vector<uint8_t>& quals)
    : std::vector<QualityValue>(quals.size())
{
    std::copy(quals.cbegin(), quals.cend(), begin());
}

QualityValues::QualityValues(const std::vector<uint8_t>::const_iterator first,
                             const std::vector<uint8_t>::const_iterator last)
    : std::vector<QualityValue>(first, last)
{}

QualityValues::QualityValues(const QualityValues::const_iterator first,
                             const QualityValues::const_iterator last)
    : std::vector<QualityValue>{}
{
    assign(first, last);
}

QualityValues& QualityValues::operator=(std::vector<QualityValue> quals) noexcept
{
    std::vector<QualityValue>::operator=(std::move(quals));
    return *this;
}

std::vector<QualityValue>::const_iterator QualityValues::cbegin() const
{
    return std::vector<QualityValue>::cbegin();
}

std::vector<QualityValue>::const_iterator QualityValues::cend() const
{
    return std::vector<QualityValue>::cend();
}

std::vector<QualityValue>::const_iterator QualityValues::begin() const
{
    return std::vector<QualityValue>::begin();
}

std::vector<QualityValue>::const_iterator QualityValues::end() const
{
    return std::vector<QualityValue>::end();
}

std::vector<QualityValue>::iterator QualityValues::begin()
{
    return std::vector<QualityValue>::begin();
}

std::vector<QualityValue>::iterator QualityValues::end()
{
    return std::vector<QualityValue>::end();
}

QualityValues QualityValues::FromFastq(const std::string& fastq) { return QualityValues{fastq}; }

std::string QualityValues::Fastq() const
{
    std::string result;
    result.resize(size());
    int32_t counter = 0;
    for (const auto& qv : *this) {
        result[counter++] = qv.Fastq();
    }
    return result;
}

bool QualityValues::operator==(const std::string& fastq) const noexcept
{
    return *this == QualityValues(fastq);
}

bool QualityValues::operator!=(const std::string& fastq) const noexcept
{
    return *this != QualityValues(fastq);
}

std::ostream& operator<<(std::ostream& os, const QualityValues& qualityvalues)
{
    return os << "QualityValues(Fastq=" << qualityvalues.Fastq() << ')';
}

}  // namespace Data
}  // namespace PacBio

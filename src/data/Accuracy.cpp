#include <pbcopper/data/Accuracy.h>

#include <type_traits>

#include <boost/algorithm/clamp.hpp>

namespace PacBio {
namespace Data {

const float Accuracy::MINUS_ONE = -1.0f;
const float Accuracy::MIN = 0.0f;
const float Accuracy::MAX = 1.0f;

Accuracy::Accuracy(float accuracy)
    : accuracy_{(accuracy == Accuracy::MINUS_ONE)
                    ? Accuracy::MINUS_ONE
                    : boost::algorithm::clamp(accuracy, Accuracy::MIN, Accuracy::MAX)}
{
}

Accuracy::operator float() const noexcept { return accuracy_; }

}  // namespace Data
}  // namespace PacBio

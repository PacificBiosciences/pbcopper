#ifndef PBCOPPER_CLI_OPTIONFLAGS_H
#define PBCOPPER_CLI_OPTIONFLAGS_H

namespace PacBio {
namespace CLI {

///
/// \brief The OptionFlags enum defines configuration switches for an Option.
///
enum OptionFlags
{
    DEFAULT        = 0x00  ///< No special configuration desired. Use defaults.
  , HIDE_FROM_HELP = 0x01  ///< Exclude option from command-line help display.
};

///
/// \return an OptionFlags value containing the result of the bitwise-OR on
///         input flags
///
constexpr OptionFlags operator|(const OptionFlags lhs, const OptionFlags rhs)
{
    return static_cast<OptionFlags>(static_cast<int>(lhs) |
                                    static_cast<int>(rhs));
}

} // namespace CLI
} // namespace PacBio

# endif // PBCOPPER_CLI_OPTIONFLAGS_H

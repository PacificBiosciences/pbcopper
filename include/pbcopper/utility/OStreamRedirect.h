#ifndef PBCOPPER_UTILITY_OSTREAMREDIRECT_H
#define PBCOPPER_UTILITY_OSTREAMREDIRECT_H

#include <pbcopper/PbcopperConfig.h>

#include <iostream>
#include <ostream>
#include <streambuf>

namespace PacBio {
namespace Utility {

// clang-format off

///
/// \internal
///
struct OStreamRedirectBase
{
public:
    virtual ~OStreamRedirectBase() { s_.rdbuf(oldBuffer_); }

protected:
    OStreamRedirectBase(std::ostream& o, std::streambuf* newBuffer)
        : s_{o}, oldBuffer_{o.rdbuf()}
    {
        o.rdbuf(newBuffer);
    }

private:
    std::ostream& s_;
    std::streambuf* oldBuffer_;
};

///
/// Redirect std::cout into the stream buffer provided. std::cout is restored on
/// CoutRedirect destruction.
///
/// \code
/// std::ostringstream stream;
/// {
///     Utility::CoutRedirect redirect{stream.rdbuf()};
///
///     // Anything normally written to std::cout in this scope will be
///     // written to stream instead.
/// }
/// \endcode
///
struct CoutRedirect : public OStreamRedirectBase
{
public:
    CoutRedirect(std::streambuf* newBuffer)
        : OStreamRedirectBase{std::cout, newBuffer}
    {}
};

///
/// Redirect std::cerr into the stream buffer provided. std::cerr is restored on
/// CerrRedirect destruction.
///
/// \code
/// std::ostringstream stream;
/// {
///     Utility::CerrRedirect redirect{stream.rdbuf()};
///
///     // Anything normally written to std::cerr in this scope will be
///     // written to stream instead.
/// }
/// \endcode
///
struct CerrRedirect : public OStreamRedirectBase
{
public:
    CerrRedirect(std::streambuf* newBuffer)
        : OStreamRedirectBase{std::cerr, newBuffer}
    {}
};

// clang-format on

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_OSTREAMREDIRECT_H

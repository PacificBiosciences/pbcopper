#ifndef PBCOPPER_TESTS_OSTREAMREDIRECTOR_H
#define PBCOPPER_TESTS_OSTREAMREDIRECTOR_H

#include <iostream>

namespace tests {

//
// Redirects specified stream into some newBuffer, restoring the normal stream
// on destruction
//
struct OStreamRedirect
{
public:
    OStreamRedirect(std::ostream& o,
                    std::streambuf* newBuffer)
        : s_(o)
        , oldBuffer_(o.rdbuf())
    { o.rdbuf(newBuffer); }

    virtual ~OStreamRedirect()
    { s_.rdbuf(oldBuffer_); }

private:
    std::ostream& s_;
    std::streambuf* oldBuffer_;
};

// Convenience OStreamRedirect for std::cout
//
struct CoutRedirect : public OStreamRedirect
{
public:
    CoutRedirect(std::streambuf* newBuffer)
        : OStreamRedirect(std::cout, newBuffer)
    { }
};

// Convenience OStreamRedirect for std::cerr
//
struct CerrRedirect : public OStreamRedirect
{
public:
    CerrRedirect(std::streambuf* newBuffer)
        : OStreamRedirect(std::cerr, newBuffer)
    { }
};

} // namespace tests

#endif // PBCOPPER_TESTS_OSTREAMREDIRECTOR_H

// Author: Derek Barnett

#ifndef PBCOPPER_LIBRARY_BUNDLE_H
#define PBCOPPER_LIBRARY_BUNDLE_H

#include <pbcopper/PbcopperConfig.h>

#include <memory>
#include <vector>

#include <pbcopper/library/Info.h>

namespace PacBio {
namespace Library {

struct Bundle
{
    // "this" parent library
    Library::Info Library;

    // child dependency bundles
    std::vector<std::unique_ptr<Library::Bundle>> Dependencies;

    // Add child dependency bundle
    Bundle& operator+=(Library::Bundle bundle)
    {
        Dependencies.emplace_back(std::make_unique<Library::Bundle>(std::move(bundle)));
        return *this;
    }

    // Add child dependency (single library)
    Bundle& operator+=(Library::Info library)
    {
        *this += Library::Bundle{library, {}};
        return *this;
    }
};

}  // namespace Library
}  // namespace PacBio

#endif  // PBCOPPER_LIBRARY_BUNDLE_H
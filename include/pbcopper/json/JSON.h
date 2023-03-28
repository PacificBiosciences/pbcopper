#ifndef PBCOPPER_JSON_JSON_H
#define PBCOPPER_JSON_JSON_H

#include <pbcopper/PbcopperConfig.h>

// disable float-equal warnings on GCC/clang
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlogical-op"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#endif

// disable documentation warnings on clang
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#endif

#include "../third-party/json.hpp"

// restore GCC/clang diagnostic settings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma GCC diagnostic pop
#endif

//
// Simple wrapper around nlohmann's JSON implementation:
//     https://github.com/nlohmann/json
//
// Currently wrapping commit: 104c5c19964124eceb742fbd84ee63ef0ac0d37d
//

namespace PacBio {
namespace JSON {

using Json = nlohmann::json;
using JsonConstIterator = nlohmann::detail::iter_impl<const nlohmann::json>;
using JsonIterator = nlohmann::detail::iter_impl<nlohmann::json>;

}  // namespace JSON
}  // namespace PacBio

#endif  // PBCOPPER_JSON_JSON_H

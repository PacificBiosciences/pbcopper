#ifndef PBCOPPER_JSON_JSON_H
#define PBCOPPER_JSON_JSON_H

#include "pbcopper/json/internal/json.hpp"

//
// Simple wrapper around nlohmann's JSON implementation:
//     https://github.com/nlohmann/json
//
// Currently wrapping commit: a3f432b3dde1d71c8940961947db5938ed11ee91
//

namespace PacBio {
namespace JSON {

typedef nlohmann::json Json;

} // namespace JSON
} // namespace PacBio

#endif // PBCOPPER_JSON_JSON_H

#include <pbcopper/LibraryInfo.h>

#include "LibraryGitHash.h"
#include "LibraryVersion.h"

#include <boost/algorithm/string.hpp>
#include <boost/version.hpp>

#include <string>

namespace PacBio {
namespace Pbcopper {

Library::Info BoostLibraryInfo()
{
    std::string release = BOOST_LIB_VERSION;
    boost::replace_all(release, "_", ".");
    return {"boost", release, ""};
}

Library::Bundle LibraryBundle()
{
    Library::Bundle bundle{LibraryInfo(), {}};
    bundle += BoostLibraryInfo();
    return bundle;
}

Library::Info LibraryInfo()
{
    return {"pbcopper", Pbcopper::ReleaseVersion, Pbcopper::LibraryGitSha1};
}

}  // namespace Pbcopper
}  // namespace PacBio

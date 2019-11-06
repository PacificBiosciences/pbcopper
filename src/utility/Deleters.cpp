// Author: Derek Barnett

#include <pbcopper/utility/Deleters.h>

#include <cstdlib>

namespace PacBio {
namespace Utility {

void FileDeleter::operator()(std::FILE* fp) const
{
    if (fp) std::fclose(fp);
    fp = nullptr;
}

void FreeDeleter::operator()(void* p) const { std::free(p); }

void FreeDeleter::operator()(const void* p) const { operator()(const_cast<void*>(p)); }

}  // namespace Utility
}  // namespace PacBio
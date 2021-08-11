#include <pbcopper/library/Bundle.h>
#include <pbcopper/library/Info.h>

#include <ostream>
#include <sstream>

#include <gtest/gtest.h>

using namespace PacBio;

namespace LibraryTests {

void PrintLibraryInfo(std::ostream& out, const Library::Info& libInfo,
                      const std::string indent = "")
{
    out << indent << "Name:    " << libInfo.Name << '\n'
        << indent << "Release: " << libInfo.Release << '\n';
    if (!libInfo.GitSha1.empty()) {
        out << indent << "GitSHA1: " << libInfo.GitSha1 << '\n';
    }
}

void PrintLibraryBundle(std::ostream& out, const Library::Bundle& bundle,
                        const std::string indent = "")
{
    PrintLibraryInfo(out, bundle.Library, indent);
    if (!bundle.Dependencies.empty()) {
        out << indent << "Dependencies:\n\n";
        for (const auto& dep : bundle.Dependencies) {
            PrintLibraryBundle(out, *dep.get(), indent + "  ");
            out << '\n';
        }
    }
}

Library::Info Dep_1_Info() { return {"dep_1", "4.2", ""}; }
Library::Info Dep_2_Info() { return {"dep_2", "0.3", "deadbeef"}; }

Library::Bundle Lib_1_Bundle()
{
    Library::Bundle bundle;
    bundle.Library = {"library_1", "2.0", "ba5eba11"};
    bundle += Library::Info{"lib_dep", "beta", ""};
    bundle += Library::Info{"lib_dep2", "14.11", "dea110c8"};
    return bundle;
}

}  // namespace LibraryTests

TEST(Library_Info, can_create_bundle_with_layered_dependencies)
{
    Library::Bundle bundle{{"my_library", "1.6.0", "release-9.0.0-43-g3f2b583"}, {}};
    bundle += LibraryTests::Dep_1_Info();
    bundle += LibraryTests::Dep_2_Info();
    bundle += LibraryTests::Lib_1_Bundle();

    std::ostringstream out;
    LibraryTests::PrintLibraryBundle(out, bundle);

    const std::string expected{R"(
Name:    my_library
Release: 1.6.0
GitSHA1: release-9.0.0-43-g3f2b583
Dependencies:

  Name:    dep_1
  Release: 4.2

  Name:    dep_2
  Release: 0.3
  GitSHA1: deadbeef

  Name:    library_1
  Release: 2.0
  GitSHA1: ba5eba11
  Dependencies:

    Name:    lib_dep
    Release: beta

    Name:    lib_dep2
    Release: 14.11
    GitSHA1: dea110c8


)"};
    EXPECT_EQ(out.str(), expected.substr(1));
}

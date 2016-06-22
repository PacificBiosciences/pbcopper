#include "pbcopper/cli/VersionPrinter.h"
#include "pbcopper/cli/Interface.h"
#include <ostream>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

void VersionPrinter::Print(const Interface& interface, ostream& out)
{
    out << interface.ApplicationName() << " "
        << interface.ApplicationVersion() << endl;
}

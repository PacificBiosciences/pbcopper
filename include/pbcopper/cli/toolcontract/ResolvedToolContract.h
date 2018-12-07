// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_RESOLVEDTOOLCONTRACT_H
#define PBCOPPER_CLI_TOOLCONTRACT_RESOLVEDTOOLCONTRACT_H

#include <iosfwd>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/Results.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

class ResolvedToolContract
{
public:
    ResolvedToolContract(Interface interface);
    ~ResolvedToolContract();

public:
    Results Parse(std::istream& in);

private:
    class ResolvedToolContractPrivate;
    std::unique_ptr<ResolvedToolContractPrivate> d_;
};

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_RESOLVEDTOOLCONTRACT_H

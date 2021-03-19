#include "common/Shell.h"

namespace ureport
{
namespace test
{
    class ShellDummy : public Shell
    {
        void Execute(const std::string& cmd, const std::vector<std::string> args)
        {
        }
    };
}
}

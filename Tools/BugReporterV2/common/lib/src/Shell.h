#include <string>
#include <vector>

namespace ureport
{
    class Shell
    {
    public:
        std::string void Execute(const std::string& cmd, const std::vector<std::string> args) = 0;
    };
}

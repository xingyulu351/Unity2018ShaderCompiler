#include "ShellSystemProfile.h"
#include "shims/logical/IsEmpty.h"
#include "system_interplay/Shell.h"

#include <sstream>
#include <algorithm>

namespace ureport
{
namespace macosx
{
    ShellSystemProfile::ShellSystemProfile(std::shared_ptr<Shell> shell)
        : m_Shell(shell)
    {
    }

    std::string ShellSystemProfile::GetData(const std::vector<DataType>& types) const
    {
        if (IsEmpty(types))
            return "";
        std::stringstream command;
        command << "system_profiler";
        std::for_each(types.begin(), types.end(), [&] (const DataType& type)
        {
            command << " " << type;
        });
        return m_Shell->ExecuteCommand(command.str());
    }
}
}

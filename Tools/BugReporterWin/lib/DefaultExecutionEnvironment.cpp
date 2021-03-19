#include "DefaultExecutionEnvironment.h"

#include "Windows.h"

namespace unity
{
    bool DefaultExecutionEnvironment::IsDebuggerPresent() const
    {
        return ::IsDebuggerPresent() == TRUE;
    }
}

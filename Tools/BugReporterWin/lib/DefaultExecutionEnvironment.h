#pragma once

#include "ExecutionEnvironment.h"

namespace unity
{
    class DefaultExecutionEnvironment : public ExecutionEnvironment
    {
    public:
        bool IsDebuggerPresent() const;
    };
}

#pragma once

namespace unity
{
    class ExecutionEnvironment
    {
    public:
        virtual bool IsDebuggerPresent() const = 0;

        virtual ~ExecutionEnvironment() {}
    };
}

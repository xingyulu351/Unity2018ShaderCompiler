#pragma once

namespace unity
{
namespace progression
{
    class Progression;

    class Monitor
    {
    public:
        virtual void ProgressChanged() = 0;

        virtual ~Monitor() {}
    };
}
}

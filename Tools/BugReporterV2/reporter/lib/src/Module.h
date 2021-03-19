#pragma once

namespace ureport
{
    class Workspace;

    class Module
    {
    public:
        virtual void Initialize(Workspace& workspace) = 0;

        virtual ~Module() {}
    };
}

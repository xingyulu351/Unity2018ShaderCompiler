#pragma once

#include "reporter/Module.h"

namespace ureport
{
namespace test
{
    class ModuleInitializeSpy : public ureport::Module
    {
        unsigned int m_Count;

    public:
        ModuleInitializeSpy()
            : m_Count(0)
        {
        }

        unsigned int GetCallCount() const
        {
            return m_Count;
        }

        void Initialize(Workspace& workspace)
        {
            ++m_Count;
        }
    };
}
}

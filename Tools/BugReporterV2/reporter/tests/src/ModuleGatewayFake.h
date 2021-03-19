#pragma once

#include "reporter/ModuleGateway.h"

namespace ureport
{
namespace test
{
    class ModuleGatewayFake : public ureport::ModuleGateway
    {
        ModuleGateway::Modules m_Modules;

    public:

        void AddModule(ModuleRef module)
        {
            m_Modules.push_back(module);
        }

        Modules GetAllModules() const
        {
            return m_Modules;
        }
    };
}
}

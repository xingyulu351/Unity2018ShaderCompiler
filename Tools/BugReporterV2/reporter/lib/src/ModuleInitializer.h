#pragma once

#include "reporter/ModuleGateway.h"

namespace ureport
{
    template<typename M>
    class ModuleInitializer
    {
    protected:
        ModuleGateway& m_Gateway;

    protected:

        ModuleInitializer()
            : m_Gateway(GetModuleGateway())
        {
        }
    };
}

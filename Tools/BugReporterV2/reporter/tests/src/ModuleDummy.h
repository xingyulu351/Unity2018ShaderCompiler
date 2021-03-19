#pragma once

#include "reporter/Module.h"

namespace ureport
{
namespace test
{
    class ModuleDummy : public ureport::Module
    {
        void Initialize(Workspace& workspace)
        {
        }
    };
}
}

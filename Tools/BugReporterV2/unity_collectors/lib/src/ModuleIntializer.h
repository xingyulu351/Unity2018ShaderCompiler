#pragma once

#include "reporter/ModuleInitializer.h"

namespace ureport
{
namespace unity_collectors
{
    class ModuleInitializer : public ureport::ModuleInitializer<ModuleInitializer>
    {
    public:
        ModuleInitializer();
    };
}
}

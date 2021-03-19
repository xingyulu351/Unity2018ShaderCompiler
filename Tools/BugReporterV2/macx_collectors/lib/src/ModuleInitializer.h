#pragma once

#include "reporter/ModuleInitializer.h"

namespace ureport
{
namespace macx_collectors
{
    class ModuleInitializer : public ureport::ModuleInitializer<ModuleInitializer>
    {
    public:
        ModuleInitializer();
    };
}
}

#pragma once

#include "reporter/ModuleInitializer.h"

namespace ureport
{
namespace qt_exporter
{
    class ModuleInitializer : public ureport::ModuleInitializer<ModuleInitializer>
    {
    public:
        ModuleInitializer();
    };
}
}

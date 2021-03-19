#pragma once

#include "reporter/ModuleInitializer.h"

namespace ureport
{
namespace qt_sender
{
    class ModuleInitializer : public ureport::ModuleInitializer<ModuleInitializer>
    {
    public:
        ModuleInitializer();
    };
}
}

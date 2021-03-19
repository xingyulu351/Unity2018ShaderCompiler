#include "ModuleInitializer.h"
#include "OSInfoCollector.h"
#include "HardwareInfoCollector.h"
#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/Workspace.h"

namespace ureport
{
namespace system_collectors
{
    class Module : public ureport::Module
    {
        void Initialize(Workspace& workspace)
        {
            workspace.RegisterCollector(
                std::make_shared<OSInfoCollector>(CreateSystemInfoProvider()));
            workspace.RegisterCollector(
                std::make_shared<HardwareInfoCollector>(CreateHardwareInfoProvider()));
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

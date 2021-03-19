#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/Workspace.h"
#include "QtExporterModuleInitializer.h"
#include "QtExporter.h"

namespace ureport
{
namespace qt_exporter
{
    class Module : public ureport::Module
    {
        void Initialize(Workspace& workspace)
        {
            workspace.SetupExporter(CreateQtExporter());
            workspace.RegisterCollector(CreateJsonReportCollector());
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

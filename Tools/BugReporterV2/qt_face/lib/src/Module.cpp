#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/ReportWorksheet.h"
#include "reporter/Workspace.h"
#include "ModuleInitializer.h"

namespace ureport
{
namespace qt_face
{
    std::unique_ptr<ReportWorksheet> CreateWorksheet();

    class Module : public ureport::Module
    {
        void Initialize(Workspace& workspace)
        {
            workspace.SetupReportWorksheet(CreateWorksheet());
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/Workspace.h"
#include "QtSenderModuleInitializer.h"
#include "QtSender.h"

namespace ureport
{
namespace qt_sender
{
    class Module : public ureport::Module
    {
        void Initialize(Workspace& workspace)
        {
            workspace.SetupSender(CreateQtSender());
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

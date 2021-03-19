#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/ReportWorksheet.h"
#include "reporter/Workspace.h"
#include "ModuleInitializer.h"
#include "file_system/FileSystem.h"
#include "file_system/SystemFileGateway.h"
#include "attachment/AttachmentFactory.h"
#include "SystemInfoCollector.h"

namespace ureport
{
namespace win_collectors
{
    class Module : public ureport::Module
    {
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<FileGateway> m_FileGateway;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;

    public:
        Module()
            : m_FileSystem(CreateFileSystem())
            , m_FileGateway(CreateSystemFileGateway())
            , m_AttachmentFactory(CreateAttachmentFactory(m_FileSystem, m_FileGateway))
        {
        }

        void Initialize(Workspace& workspace) override
        {
            using namespace ::ureport::collectors;
            workspace.RegisterCollector(std::make_shared<SystemInfoCollector>(m_AttachmentFactory));
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

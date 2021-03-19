#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/Workspace.h"
#include "ModuleIntializer.h"
#include "UnityVersionCollector.h"
#include "UnityLogsCollector.h"
#include "UnityLicenseCollector.h"
#include "file_system/SystemFileGateway.h"
#include "file_system/FileSystem.h"
#include "system_interplay/SystemEnvironment.h"
#include "system_interplay/Gate.h"

#include <memory>

namespace ureport
{
namespace unity_collectors
{
    class Module : public ureport::Module
    {
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<FileGateway> m_FileGateway;
        std::shared_ptr<UnityVersion> m_UnityVersion;

    public:
        Module()
            : m_FileSystem(CreateFileSystem())
            , m_FileGateway(CreateSystemFileGateway())
            , m_UnityVersion(unity_version::CreateUnityVersion())
        {
        }

        void Initialize(Workspace& workspace)
        {
            workspace.RegisterCollector(
                std::make_shared<UnityVersionCollector>(m_UnityVersion));
            workspace.RegisterCollector(
                std::make_shared<UnityLogsCollector>(m_FileGateway, m_FileSystem));
            workspace.RegisterCollector(CreateUnityLicenseCollector());
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

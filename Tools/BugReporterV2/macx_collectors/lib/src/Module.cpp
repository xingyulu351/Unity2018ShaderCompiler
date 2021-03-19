#include "ModuleInitializer.h"
#include "UnityCrashLogsCollector.h"
#include "SystemProfileCollector.h"
#include "ShellSystemProfile.h"
#include "LogAttacher.h"
#include "reporter/Module.h"
#include "reporter/ModuleGateway.h"
#include "reporter/ReportWorksheet.h"
#include "reporter/Workspace.h"
#include "file_system/FileSystem.h"
#include "file_system/SystemFileGateway.h"
#include "attachment/AttachmentFactory.h"
#include "system_interplay/Gate.h"
#include "system_interplay/Shell.h"

namespace ureport
{
namespace macx_collectors
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

        void Initialize(Workspace& workspace)
        {
            using namespace macosx;
            using namespace collectors;
            const std::vector<std::string> stats({
                                                 "Hardware", "Network", "Memory", "PCI", "IDE", "SCSI", "USB", "FireWire",
                                                 "ParallelATA", "Audio", "Bluetooth", "Diagnostics", "DiscBurning", "FibreChannel",
                                                 "Displays", "PCCard", "ParallelSCSI", "Power", "SerialATA", "AirPort", "Firewall"
                                             });
            workspace.RegisterCollector(std::make_shared<UnityCrashLogsCollector>(
                std::make_shared<CrashReportsProvider>(m_FileSystem), m_AttachmentFactory));
            std::shared_ptr<Shell> shell = CreateShell();
            for (auto iter = stats.begin(); iter != stats.end(); iter++)
            {
                workspace.RegisterCollector(std::make_shared<SystemProfileCollector>(
                    std::make_shared<ShellSystemProfile>(shell), *iter));
            }
            workspace.RegisterCollector(std::make_shared<LogAttacher>(m_AttachmentFactory, stats));
        }
    };

    ModuleInitializer::ModuleInitializer()
    {
        m_Gateway.AddModule(std::make_shared<Module>());
    }
}
}

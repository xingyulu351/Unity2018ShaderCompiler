#include "Workspace.h"
#include "ModuleGateway.h"
#include "Module.h"
#include "Sender.h"
#include "Exporter.h"
#include "Collector.h"
#include "ReportWorksheet.h"
#include "WorkspaceIsNotInitializedException.h"
#include "shims/logical/IsNull.h"

#include <memory>
#include <vector>

namespace ureport
{
namespace details
{
    class Workspace : public ureport::Workspace
    {
        const ModuleGateway& m_ModuleGateway;
        std::shared_ptr<Sender> m_Sender;
        std::shared_ptr<Exporter> m_Exporter;
        std::shared_ptr<ReportWorksheet> m_Worksheet;
        std::vector<std::shared_ptr<Collector> > m_Collectors;

    public:
        Workspace(const ModuleGateway& modules)
            : m_ModuleGateway(modules)
        {
        }

        void Initialize()
        {
            InitializeModules();
        }

    private:
        void InitializeModules()
        {
            ModuleGateway::Modules modules = m_ModuleGateway.GetAllModules();
            for (ModuleGateway::Modules::const_iterator moduleIter = modules.begin();
                 moduleIter != modules.end(); ++moduleIter)
            {
                (*moduleIter)->Initialize(*this);
            }
        }

        void SetupSender(std::unique_ptr<Sender> sender)
        {
            m_Sender.reset(sender.release());
        }

        std::shared_ptr<Sender> GetSender() const
        {
            return m_Sender;
        }

        void SetupExporter(std::unique_ptr<Exporter> exporter)
        {
            m_Exporter.reset(exporter.release());
        }

        std::shared_ptr<Exporter> GetExporter() const
        {
            return m_Exporter;
        }

        void SetupReportWorksheet(std::unique_ptr<ReportWorksheet> worksheet)
        {
            m_Worksheet.reset(worksheet.release());
        }

        std::shared_ptr<ReportWorksheet> GetReportWorksheet() const
        {
            return m_Worksheet;
        }

        void RegisterCollector(std::shared_ptr<Collector> collector)
        {
            m_Collectors.push_back(collector);
        }

        std::vector<std::shared_ptr<Collector> > GetAllCollectors() const
        {
            return m_Collectors;
        }
    };

    static std::unique_ptr<Workspace> s_Workspace;
}

    const Workspace& Workspace::Initialize()
    {
        return Initialize(GetModuleGateway());
    }

    const Workspace& Workspace::Initialize(const ModuleGateway& modules)
    {
        details::s_Workspace.reset(new details::Workspace(modules));
        details::s_Workspace->Initialize();
        return GetWorkspace();
    }

    const Workspace& Workspace::GetWorkspace()
    {
        if (IsNull(details::s_Workspace))
            throw WorkspaceIsNotInitializedException();
        return *details::s_Workspace;
    }
}

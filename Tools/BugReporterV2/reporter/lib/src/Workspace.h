#pragma once

#include <memory>
#include <vector>

namespace ureport
{
    class ModuleGateway;
    class Sender;
    class Exporter;
    class Collector;
    class ReportWorksheet;

    class Workspace
    {
    public:
        static const Workspace& Initialize();

        static const Workspace& Initialize(const ModuleGateway& modules);

        static const Workspace& GetWorkspace();

        virtual void SetupSender(std::unique_ptr<Sender> sender) = 0;

        virtual std::shared_ptr<Sender> GetSender() const = 0;

        virtual void SetupExporter(std::unique_ptr<Exporter> exporter) = 0;

        virtual std::shared_ptr<Exporter> GetExporter() const = 0;

        virtual void SetupReportWorksheet(std::unique_ptr<ReportWorksheet> worksheet) = 0;

        virtual std::shared_ptr<ReportWorksheet> GetReportWorksheet() const = 0;

        virtual void RegisterCollector(std::shared_ptr<Collector> collector) = 0;

        virtual std::vector<std::shared_ptr<Collector> > GetAllCollectors() const = 0;

    protected:
        Workspace() {}

        virtual ~Workspace() {}
    };
}

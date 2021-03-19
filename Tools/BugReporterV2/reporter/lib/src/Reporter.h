#pragma once

#include <memory>

namespace ureport
{
    class Sender;
    class Exporter;
    class ReportWorksheet;
    class FilesPacker;
    class Collector;
    class ReportDraft;
    class Settings;
    class ReportModifier;

    class Reporter
    {
    public:
        virtual void SetWorksheet(std::shared_ptr<ReportWorksheet> worksheet) = 0;

        virtual void SetSender(std::shared_ptr<Sender> sender) = 0;

        virtual void SetExporter(std::shared_ptr<Exporter> exporter) = 0;

        virtual void SetFilesPacker(std::shared_ptr<FilesPacker> packer) = 0;

        virtual void SetSettings(std::shared_ptr<Settings> settings) = 0;

        virtual void AddCollector(std::shared_ptr<Collector> collector) = 0;

        virtual void AddReportInitializer(std::shared_ptr<ReportModifier> initializer) = 0;

        virtual void DoReport(std::shared_ptr<ReportDraft> report) = 0;

        virtual ~Reporter() {}
    };

    std::unique_ptr<Reporter> CreateNewReporter();
}

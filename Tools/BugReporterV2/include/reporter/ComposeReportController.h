#pragma once

#include "file_system/FileSystem.h"
#include "reporter/Report.h"
#include "long_term_operation/LongTermOperObserver.h"

#include <memory>
#include <string>
#include <functional>
#include <exception>

namespace ureport
{
    class ReportDraft;
    class Attachment;
    class ProgressiveOperation;
    class LongTermOperObserver;

    class ComposeReportController
    {
    public:
        virtual void SetReport(std::shared_ptr<ReportDraft> report) = 0;

        virtual void SetTitle(const std::string& title) = 0;

        virtual void SetTextualDescription(const std::string& description) = 0;

        virtual void SetReporterEmail(const std::string& email) = 0;

        virtual void SetBugTypeID(const Report::BugTypeID id) = 0;

        virtual void SetBugReproducibility(
            const Report::BugReproducibility repro) = 0;

        virtual void AttachFile(const std::string& path) = 0;

        virtual void AttachDirectory(const std::string& path) = 0;

        virtual void AttachReportData() = 0;

        virtual void RemoveAttachment(std::shared_ptr<Attachment> attachment) = 0;

        virtual void PrepareReport(LongTermOperObserver*) = 0;

        virtual void SendReport(LongTermOperObserver*) = 0;

        virtual void SaveReport(LongTermOperObserver*, const std::string& path) = 0;

        virtual void WriteFact(const FactName& name, const Fact& fact) = 0;

        virtual ~ComposeReportController() {}
    };

    class AttachmentFactory;

    std::unique_ptr<ComposeReportController> CreateComposeReportController(
        std::shared_ptr<AttachmentFactory> factory,
        std::shared_ptr<FileSystem> fileSystem,
        std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
        std::function<void(Report&, LongTermOperObserver*)> sendReport,
        std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport);
}

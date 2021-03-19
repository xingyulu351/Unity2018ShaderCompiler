#pragma once

#include "reporter/ComposeReportController.h"

namespace ureport
{
namespace test
{
    class ComposeReportControllerDummy : public ureport::ComposeReportController
    {
        void SetReport(std::shared_ptr<ReportDraft> report) {}

        void SetTitle(const std::string& title) {}

        void SetTextualDescription(const std::string& description) {}

        void SetReporterEmail(const std::string& email) {}

        void SetBugTypeID(const Report::BugTypeID) {}

        void SetBugReproducibility(const Report::BugReproducibility) {}

        void AttachFile(const std::string& path) {}

        void AttachDirectory(const std::string& path) {}

        void AttachReportData() {}

        void RemoveAttachment(std::shared_ptr<Attachment> attachment) {}

        virtual void PrepareReport(LongTermOperObserver*) {}

        virtual void SendReport(LongTermOperObserver*) {}

        virtual void SaveReport(LongTermOperObserver* observer, const std::string& path) {}

        void WriteFact(const FactName& name, const Fact& fact) {}
    };
}
}

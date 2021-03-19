#pragma once

#include "Report.h"

namespace ureport
{
    class Attachment;

    class ReportDraft : public Report
    {
    public:
        virtual void SetTitle(const std::string& title) = 0;

        virtual void SetTextualDescription(const std::string& text) = 0;

        virtual void SetReporterEmail(const std::string& email) = 0;

        virtual void SetBugTypeID(const Report::BugTypeID id) = 0;

        virtual void SetBugReproducibility(
            const Report::BugReproducibility repro) = 0;

        virtual void WriteFact(const FactName& name, const Fact& fact) = 0;

        virtual void Attach(std::shared_ptr<Attachment> attachment) = 0;

        virtual void Detach(std::shared_ptr<Attachment> attachment) = 0;
    };

    std::unique_ptr<ReportDraft> CreateNewReport();
}

#pragma once

#include "reporter/Report.h"
#include "HumbleAttachment.h"

namespace ureport
{
namespace test
{
    class HumbleReport : public ureport::Report
    {
        std::string GetTitle() const
        {
            return "REPORT";
        }

        std::string GetTextualDescription() const
        {
            return "DESCRIPTION";
        }

        std::string GetReporterEmail() const
        {
            return "user@site.org";
        }

        BugTypeID GetBugTypeID() const
        {
            return kBugProblemWithEditor;
        }

        BugReproducibility GetBugReproducibility() const
        {
            return kReproAlways;
        }

        std::string ReadFact(const FactName& name) const
        {
            return "FACT";
        }

        std::list<std::shared_ptr<Attachment> > GetAttachments() const
        {
            std::list<std::shared_ptr<Attachment> > attachments;
            attachments.push_back(std::make_shared<HumbleAttachment>());
            return attachments;
        }

        void AddMonitor(std::shared_ptr<ReportMonitor> monitor)
        {
        }
    };
}
}

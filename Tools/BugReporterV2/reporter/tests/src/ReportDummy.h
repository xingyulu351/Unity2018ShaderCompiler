#pragma once

#include "reporter/Report.h"

namespace ureport
{
namespace test
{
    class ReportDummy : public ureport::Report
    {
        std::string GetTitle() const
        {
            return std::string();
        }

        std::string GetTextualDescription() const
        {
            return std::string();
        }

        std::string GetReporterEmail() const
        {
            return std::string();
        }

        BugTypeID GetBugTypeID() const
        {
            return kBugProblemWithEditor;
        }

        BugReproducibility GetBugReproducibility() const
        {
            return kReproFirstTime;
        }

        std::string ReadFact(const FactName& name) const
        {
            return std::string();
        }

        std::list<std::shared_ptr<Attachment> > GetAttachments() const
        {
            return std::list<std::shared_ptr<Attachment> >();
        }

        void AddMonitor(std::shared_ptr<ReportMonitor> monitor)
        {
        }
    };
}
}

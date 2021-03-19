#pragma once

#include "reporter/ReportDraft.h"

#include <vector>
#include <tuple>

namespace ureport
{
namespace test
{
    class ReportFactSpy : public ureport::ReportDraft
    {
    public:
        std::vector<std::tuple<std::string, std::string> > m_Facts;

        std::string GetTitle() const
        {
            return std::string();
        }

        void SetTitle(const std::string& title)
        {
        }

        std::string GetTextualDescription() const
        {
            return std::string();
        }

        void SetTextualDescription(const std::string& text)
        {
        }

        std::string GetReporterEmail() const
        {
            return std::string();
        }

        void SetReporterEmail(const std::string& email)
        {
        }

        BugTypeID GetBugTypeID() const
        {
            return kBugProblemWithEditor;
        }

        void SetBugTypeID(const Report::BugTypeID)
        {
        }

        BugReproducibility GetBugReproducibility() const
        {
            return kReproFirstTime;
        }

        void SetBugReproducibility(const Report::BugReproducibility)
        {
        }

        std::string ReadFact(const FactName& name) const
        {
            return std::string();
        }

        void WriteFact(const FactName& name, const Fact& fact)
        {
            m_Facts.push_back(std::make_tuple(name, fact));
        }

        void Attach(std::shared_ptr<Attachment> attachment)
        {
        }

        void Detach(std::shared_ptr<Attachment> attachment)
        {
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

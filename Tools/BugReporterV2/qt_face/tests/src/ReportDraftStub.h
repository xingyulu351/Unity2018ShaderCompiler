#include "reporter/ReportDraft.h"
#include <map>

namespace ureport
{
namespace test
{
    class ReportDraftStub : public ReportDraft
    {
        std::map<FactName, Fact> m_Facts;
        std::list<std::shared_ptr<Attachment> > m_Attachments;
        BugTypeID m_BugTypeId;

    public:
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
            return m_BugTypeId;
        }

        void SetBugTypeID(const Report::BugTypeID bugTypeId)
        {
            m_BugTypeId = bugTypeId;
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
            auto factIter = m_Facts.find(name);
            if (factIter == m_Facts.end())
                return std::string();
            return factIter->second;
        }

        void WriteFact(const FactName& name, const Fact& fact)
        {
            m_Facts[name] = fact;
        }

        void Attach(std::shared_ptr<Attachment> attachment)
        {
            m_Attachments.push_back(attachment);
        }

        void Detach(std::shared_ptr<Attachment> attachment)
        {
            m_Attachments.remove(attachment);
        }

        std::list<std::shared_ptr<Attachment> > GetAttachments() const
        {
            return m_Attachments;
        }

        void AddMonitor(std::shared_ptr<ReportMonitor> monitor)
        {
        }
    };
}
}

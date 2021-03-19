#include "ReportDraft.h"
#include "reporter/ReportMonitor.h"
#include "IncorrectFactNameException.h"
#include "attachment/Attachment.h"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

namespace ureport
{
namespace details
{
    class ReportDraft : public ureport::ReportDraft
    {
    public:
        ReportDraft()
            : m_BugTypeID(kBugUnknown)
            , m_BugReproducibility(kReproUnknown)
        {}

    private:
        typedef std::map<FactName, Fact> FactsMap;

        std::string GetTitle() const
        {
            return m_Title;
        }

        void SetTitle(const std::string& title)
        {
            m_Title = title;
            NotifyMonitors();
        }

        std::string GetTextualDescription() const
        {
            return m_Description;
        }

        void SetTextualDescription(const std::string& text)
        {
            m_Description = text;
            NotifyMonitors();
        }

        std::string GetReporterEmail() const
        {
            return m_ReporterEmail;
        }

        void SetReporterEmail(const std::string& email)
        {
            m_ReporterEmail = email;
            NotifyMonitors();
        }

        BugTypeID GetBugTypeID() const
        {
            return m_BugTypeID;
        }

        void SetBugTypeID(const BugTypeID id)
        {
            m_BugTypeID = id;
            NotifyMonitors();
        }

        BugReproducibility GetBugReproducibility() const
        {
            return m_BugReproducibility;
        }

        void SetBugReproducibility(const BugReproducibility repro)
        {
            m_BugReproducibility = repro;
            NotifyMonitors();
        }

        std::string ReadFact(const FactName& name) const
        {
            if (name.empty())
                throw IncorrectFactNameException();
            auto factIter = m_Facts.find(name);
            if (factIter == m_Facts.end())
                return std::string();
            return factIter->second;
        }

        void WriteFact(const FactName& name, const Fact& fact)
        {
            m_Facts[name] = fact;
            NotifyMonitors();
        }

        void Attach(std::shared_ptr<Attachment> attachment)
        {
            m_Attachments.push_back(attachment);
            NotifyMonitors();
        }

        void Detach(std::shared_ptr<Attachment> attachment)
        {
            m_Attachments.remove(attachment);
            NotifyMonitors();
        }

        std::list<std::shared_ptr<Attachment> > GetAttachments() const
        {
            return m_Attachments;
        }

        void AddMonitor(std::shared_ptr<ReportMonitor> monitor)
        {
            m_Monitors.push_back(monitor);
        }

        void NotifyMonitors()
        {
            std::for_each(m_Monitors.begin(), m_Monitors.end(),
                [] (std::shared_ptr<ReportMonitor>& monitor)
                {
                    monitor->ReportUpdated();
                });
        }

    private:
        std::string m_Title;
        std::string m_Description;
        std::string m_ReporterEmail;
        Report::BugTypeID m_BugTypeID;
        Report::BugReproducibility m_BugReproducibility;
        FactsMap m_Facts;
        std::list<std::shared_ptr<Attachment> > m_Attachments;
        std::vector<std::shared_ptr<ReportMonitor> > m_Monitors;
    };
}

    std::unique_ptr<ReportDraft> CreateNewReport()
    {
        std::unique_ptr<ReportDraft> report(new details::ReportDraft());
        return report;
    }
}

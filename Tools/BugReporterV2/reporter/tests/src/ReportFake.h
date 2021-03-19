#pragma once

#include "ReportDummy.h"
#include "attachment/Attachment.h"

#include <string>
#include <map>

namespace ureport
{
namespace test
{
    class DescriptionReportFake : public ReportDummy
    {
    public:
        std::string m_Description;

        std::string GetTextualDescription() const
        {
            return m_Description;
        }
    };

    class TitleReportFake : public ReportDummy
    {
    public:
        std::string m_Title;

        std::string GetTitle() const
        {
            return m_Title;
        }
    };

    class TitleAndDescriptionReportFake : public ReportDummy
    {
    public:
        std::string m_Description;
        std::string m_Title;

        std::string GetTextualDescription() const
        {
            return m_Description;
        }

        std::string GetTitle() const
        {
            return m_Title;
        }
    };

    class EmailReportFake : public ReportDummy
    {
    public:
        std::string m_Email;

        std::string GetReporterEmail() const
        {
            return m_Email;
        }
    };

    class BugTypeIDReportFake : public ReportDummy
    {
    public:
        BugTypeID m_BugTypeID;

        BugTypeID GetBugTypeID() const
        {
            return m_BugTypeID;
        }
    };

    class BugReproReportFake : public ReportDummy
    {
    public:
        BugReproducibility m_BugRepro;

        BugReproducibility GetBugReproducibility() const
        {
            return m_BugRepro;
        }
    };

    class FilesReportFake : public ReportDummy
    {
    public:
        std::list<std::shared_ptr<Attachment> > m_Attachments;

        std::list<std::shared_ptr<Attachment> > GetAttachments() const
        {
            return m_Attachments;
        }
    };

    class FactsReportFake : public ReportDummy
    {
    public:
        mutable std::map<std::string, std::string> m_Facts;

        std::string ReadFact(const FactName& name) const
        {
            return m_Facts[name];
        }
    };
}
}

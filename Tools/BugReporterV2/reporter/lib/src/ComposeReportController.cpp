#include "reporter/ComposeReportController.h"
#include "ReportDraft.h"
#include "attachment/AttachmentFactory.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

#ifdef LINUX
// Linux libstdc++ chokes on the longer regex (as of g++ 4.7)
#define EMAIL_REGEX ".*@.*"
#else
#define EMAIL_REGEX "^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$"
#endif

namespace ureport
{
namespace details
{
    class ComposeReportControllerImpl : public ureport::ComposeReportController
    {
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
        std::shared_ptr<ReportDraft> m_Report;
        std::shared_ptr<FileSystem> m_FileSystem;
        std::function<void(ReportDraft&, LongTermOperObserver*)> m_PrepareReport;
        std::function<void(Report&, LongTermOperObserver*)> m_SendReport;
        std::function<void(Report&, LongTermOperObserver*, const std::string&)> m_SaveReport;
        bool m_IsReportPrepared;

    public:
        ComposeReportControllerImpl(
            std::shared_ptr<AttachmentFactory> factory,
            std::shared_ptr<FileSystem> fileSystem,
            std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
            std::function<void(Report&, LongTermOperObserver*)> sendReport,
            std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport)
            : m_AttachmentFactory(factory)
            , m_FileSystem(fileSystem)
            , m_PrepareReport(prepareReport)
            , m_SendReport(sendReport)
            , m_SaveReport(saveReport)
            , m_IsReportPrepared(false)
        {
        }

        void SetReport(std::shared_ptr<ReportDraft> report)
        {
            m_Report = report;
        }

        void SetTitle(const std::string& title)
        {
            m_Report->SetTitle(title);
        }

        void SetTextualDescription(const std::string& description)
        {
            m_Report->SetTextualDescription(description);
        }

        void SetReporterEmail(const std::string& email)
        {
            m_Report->SetReporterEmail(email);
        }

        void SetBugTypeID(const Report::BugTypeID id)
        {
            m_Report->SetBugTypeID(id);
        }

        void SetBugReproducibility(const Report::BugReproducibility repro)
        {
            m_Report->SetBugReproducibility(repro);
        }

        void AttachFile(const std::string& path)
        {
            auto attachment = m_AttachmentFactory->CreateFileAttachment(
                path, "Manually attached file");
            attachment->GetTextPreview()->SetRemovable(true);
            m_Report->Attach(std::move(attachment));
        }

        void AttachDirectory(const std::string& path)
        {
            if (path.empty())
                throw std::logic_error("No path specified");
            else if (m_FileSystem->GetPathDepth(path) == 0)
                throw std::logic_error("Cannot attach root directory");

            auto attachment = m_AttachmentFactory->CreateDirectoryAttachment(
                path, "Manually attached directory");
            attachment->GetTextPreview()->SetRemovable(true);
            m_Report->Attach(std::move(attachment));
        }

        void AttachReportData()
        {
            const std::string data = m_Report->ReadFact("JsonReport");
            if (!data.empty())
            {
                std::shared_ptr<Attachment> attachment = m_AttachmentFactory->CreateTextAttachment(data, "report.json");
                attachment->GetTextPreview()->SetRemovable(false);
                m_Report->Attach(attachment);
            }
        }

        void RemoveAttachment(std::shared_ptr<Attachment> attachment)
        {
            m_Report->Detach(attachment);
        }

        void PrepareReport(LongTermOperObserver* observer)
        {
            if (!m_IsReportPrepared)
            {
                m_PrepareReport(*m_Report, observer);
                if (observer->IsOperationCanceled())
                    return;
                m_IsReportPrepared = true;
            }
        }

        void SendReport(LongTermOperObserver* observer)
        {
            CheckRequiredFields();
            PrepareReport(observer);
            m_SendReport(*m_Report, observer);
        }

        void SaveReport(LongTermOperObserver* observer, const std::string& path)
        {
            CheckRequiredFields();
            if (IsFieldEmpty(path))
                throw std::logic_error("Please select a file to save to.");
            PrepareReport(observer);
            m_SaveReport(*m_Report, observer, path);
        }

        void WriteFact(const FactName& name, const Fact& fact)
        {
            m_Report->WriteFact(name, fact);
        }

        void CheckRequiredFields() const
        {
            if (IsFieldEmpty(m_Report->GetTitle()))
                throw std::logic_error("Please write a title.");
            if (IsFieldEmpty(m_Report->GetReporterEmail()))
                throw std::logic_error("Please write your email address.");
            if (!IsEmailCorrect(m_Report->GetReporterEmail()))
                throw std::logic_error("Please write a valid email address.");
            if (m_Report->GetBugTypeID() == Report::kBugUnknown)
                throw std::logic_error("Please select what the problem is related to.");
            if (m_Report->GetBugReproducibility() == Report::kReproUnknown)
                throw std::logic_error("Please select how often the problem is happening.");
        }

        static bool IsFieldEmpty(const std::string& field)
        {
            return
                field.empty() ||
                std::all_of(field.begin(), field.end(), isspace);
        }

        bool IsEmailCorrect(const std::string& email) const
        {
            std::regex base_regex(EMAIL_REGEX);
            return std::regex_match(email, base_regex);
        }
    };
}

    std::unique_ptr<ComposeReportController> CreateComposeReportController(
        std::shared_ptr<AttachmentFactory> factory,
        std::shared_ptr<FileSystem> fileSystem,
        std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
        std::function<void(Report&, LongTermOperObserver*)> sendReport,
        std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport)
    {
        return std::unique_ptr<ComposeReportController>(
            new details::ComposeReportControllerImpl(
                factory, fileSystem, prepareReport, sendReport, saveReport));
    }
}

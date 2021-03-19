#include "reporter/UnityBugMessage.h"
#include "reporter/Report.h"
#include "file_system/File.h"
#include "reporter/FilesPacker.h"
#include "attachment/Attachment.h"
#include "common/StringUtils.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "shims/logical/IsEmpty.h"

#include <memory>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace ureport
{
namespace details
{
    class NewNameFileDecorator : public File
    {
        std::shared_ptr<File> m_Original;
        std::string m_NewName;

    public:

        NewNameFileDecorator(std::shared_ptr<File> original, const std::string& name)
            : m_Original(original)
            , m_NewName(name)
        {
        }

        std::string GetPath() const override
        {
            return m_NewName;
        }

        bool IsDir() const override
        {
            return m_Original->IsDir();
        }

        std::unique_ptr<std::istream> Read() const override
        {
            return m_Original->Read();
        }

        void Write(std::istream& data) override
        {
            m_Original->Write(data);
        }
    };

    std::string ComposeEvent(const Report& report)
    {
        std::stringstream event;
        const std::string title = report.GetTitle();
        if (!IsEmpty(title))
            event << title << "\n\n";
        event << report.GetTextualDescription();
        const std::string publicStatus = report.ReadFact("Publicity");
        if (!IsEmpty(publicStatus))
            event << "\n\nPublic status: " << publicStatus;
        const std::string licenseType = report.ReadFact("LicenseType");
        if (!IsEmpty(licenseType))
            event << "\n\nLicense type: " << licenseType;
        return event.str();
    }

    class AttachmentsPacker
    {
        std::unordered_map<std::string, size_t> m_Filenames;
        FilesPacker& m_Packer;
        LongTermOperObserver& m_Observer;

    public:
        AttachmentsPacker(FilesPacker& packer, LongTermOperObserver& observer)
            : m_Packer(packer)
            , m_Observer(observer)
        {
        }

        std::unique_ptr<File> Pack(const Report& report)
        {
            m_Observer.ReportProgress(0, 2);
            auto files = PackAttachments(report.GetAttachments());
            m_Observer.ReportProgress(1, 2);
            std::unique_ptr<File> package;
            if (!files.empty())
                package = m_Packer.Pack(files, &m_Observer);
            m_Observer.ReportProgress(2, 2);
            return package;
        }

    private:
        std::vector<std::shared_ptr<File> > PackAttachments(
            const std::list<std::shared_ptr<Attachment> >& attachments)
        {
            LongTermBlockedOperObserver helper(&m_Observer);
            helper.ReportStatus("Packing the report...");
            const size_t attTotal = attachments.size();
            helper.ReportProgress(0, attTotal);
            std::vector<std::shared_ptr<File> > files;
            size_t aTotal = 0;
            for (auto it = attachments.begin(); it != attachments.end(); ++it, ++aTotal)
            {
                if (helper.IsOperationCanceled())
                    break;
                auto file = PackAttachment(*(*it));
                if (file)
                {
                    std::string filename = file->GetPath();
                    if (NameInUse(filename))
                        files.push_back(std::make_shared<NewNameFileDecorator>(file,
                            MakeNewName(filename)));
                    else
                        files.push_back(file);
                    RememberFilename(filename);
                }
                helper.ReportProgress(aTotal + 1, attTotal);
            }
            return files;
        }

        std::shared_ptr<File> PackAttachment(Attachment& attachment)
        {
            return attachment.Pack(m_Packer, &m_Observer);
        }

        void RememberFilename(const std::string& name)
        {
            m_Filenames[name] += 1;
        }

        std::string MakeNewName(const std::string& name) const
        {
            return name + "(" + std::to_string((long long)m_Filenames.at(name) + 1) + ")";
        }

        bool NameInUse(const std::string& name) const
        {
            return m_Filenames.find(name) != m_Filenames.end();
        }
    };

    std::string ComposeComputer(const Report& report)
    {
        std::vector<std::string> facts;
        facts.push_back(report.ReadFact("BasicHardwareType"));
        facts.push_back(report.ReadFact("GraphicsHardwareType"));
        facts.push_back(report.ReadFact("OSName"));
        facts.push_back(report.ReadFact("UnityEditorMode"));
        return Join(facts.begin(), facts.end(), "; ");
    }

    std::string CreateTimestamp()
    {
        char ts_buf[32];
        std::time_t t = std::time(nullptr);
        std::ostringstream stream;

        stream.imbue(std::locale::classic());
        std::strftime(ts_buf, sizeof(ts_buf), "%Y-%m-%d %H:%M:%S", std::gmtime(&t));
        stream << ts_buf;

        return stream.str();
    }

    std::string ComposeBugReproducibility(
        const Report::BugReproducibility repro)
    {
        switch (repro)
        {
            case Report::kReproAlways:
                return "Always";
            case Report::kReproSometimes:
                return "Sometimes, but not always";
            case Report::kReproFirstTime:
                return "This is the first time";
            default:
                return "Unknown";
        }
    }
}

    UnityBugMessage UnityBugMessage::Compose(const Report& report, FilesPacker& packer,
        LongTermOperObserver* observer)
    {
        UnityBugMessage message;
        message.m_Event = details::ComposeEvent(report);
        message.m_UnityVersion = report.ReadFact("UnityVersion");
        message.m_CustomerEmail = report.GetReporterEmail();
        message.m_Computer = details::ComposeComputer(report);
        details::AttachmentsPacker attachmentsPacker(packer, *observer);
        message.m_AttachedPackedFile = attachmentsPacker.Pack(report);
        message.m_Timestamp = details::CreateTimestamp();
        message.m_BugTypeID = static_cast<int>(report.GetBugTypeID());
        message.m_BugReproducibility = details::ComposeBugReproducibility(
            report.GetBugReproducibility());
        return message;
    }

    std::string UnityBugMessage::GetAttachedFilePath() const
    {
        return m_AttachedPackedFile ? m_AttachedPackedFile->GetPath() : std::string();
    }
}

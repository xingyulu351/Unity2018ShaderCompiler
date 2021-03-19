#include "TextAttachment.h"
#include "attachment/AttachmentProperties.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "file_system/File.h"
#include "shims/logical/IsEmpty.h"
#include "system_interplay/Gate.h"
#include "system_interplay/SystemEnvironment.h"
#include "common/StringUtils.h"

#include <sstream>
#include <fstream>

namespace ureport
{
namespace details
{
    class TextAttachmentPreview : public ureport::AttachmentTextPreview
    {
        std::string m_Name;
        std::string m_Text;

    public:
        TextAttachmentPreview(const std::string& name, const std::string& text)
            : m_Name(name)
            , m_Text(text)
        {
        }

        std::string GetName() const override
        {
            return m_Name;
        }

        std::vector<std::string> GetContent() const override
        {
            auto content = std::vector<std::string>();
            if (IsEmpty(m_Text))
                return content;
            std::stringstream stream(m_Text);
            while (!stream.eof())
            {
                std::string line;
                std::getline(stream, line);
                content.push_back(line);
            }
            return content;
        }
    };

    class TextAttachmentFile : public ureport::File
    {
        std::string m_Name;
        std::string m_Text;
        std::string m_Path;

    public:
        TextAttachmentFile(const std::string& name, const std::string& text, const std::string& path)
            : m_Name(name)
            , m_Text(text)
            , m_Path(path)
        {
        }

        std::string GetPath() const override
        {
            return m_Name;
        }

        bool IsDir() const override
        {
            return false;
        }

        std::unique_ptr<std::istream> Read() const override
        {
            return std::unique_ptr<std::istream>(new std::ifstream(m_Path, std::ios::in | std::ios::binary));
        }

        void Write(std::istream& data) override
        {
        }
    };
}

    TextAttachment::TextAttachment(const std::string& name, const std::string& text)
        : m_Preview(std::make_shared<details::TextAttachmentPreview>(name, text))
    {
        std::shared_ptr<ureport::SystemEnvironment> env = ureport::CreateSystemEnvironment();
        std::string tmpFilePath = env->GetTempDirectory() + "/" + MakeRandomString(8, "0123456789abcdef");
        std::ofstream tmpTextFile;
        tmpTextFile.open(tmpFilePath);
        tmpTextFile << text;
        tmpTextFile.close();

        m_File = std::make_shared<details::TextAttachmentFile>(name, text, tmpFilePath);
    }

    std::string TextAttachment::GetProperty(const std::string& name) const
    {
        if (name == attachment::kName)
            return m_File->GetPath();
        return Attachment::GetProperty(name);
    }

    std::shared_ptr<AttachmentTextPreview> TextAttachment::GetTextPreview() const
    {
        return m_Preview;
    }

    std::shared_ptr<File> TextAttachment::Pack(FilesPacker& packer,
        LongTermOperObserver* observer)
    {
        return m_File;
    }
}

#pragma once

#include <string>
#include <vector>

namespace ureport
{
    class AttachmentTextPreview
    {
    public:
        virtual std::string GetName() const = 0;
        virtual std::vector<std::string> GetContent() const = 0;
        virtual ~AttachmentTextPreview() {}

        std::string GetDescription() const
        {
            return m_Description;
        }

        void SetDescription(const std::string& description)
        {
            m_Description = description;
        }

        bool IsRemovable() const
        {
            return m_Removable;
        }

        void SetRemovable(bool removable)
        {
            m_Removable = removable;
        }

    protected:
        std::string m_Description;
        bool m_Removable;
    };
}

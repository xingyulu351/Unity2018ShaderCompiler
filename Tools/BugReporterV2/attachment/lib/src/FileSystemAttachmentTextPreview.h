#pragma once

#include "attachment_preview/AttachmentTextPreview.h"

namespace ureport
{
    class FileSystemAttachmentTextPreview : public ureport::AttachmentTextPreview
    {
    public:
        typedef std::function<std::string(void)> GetNameFunc;
        typedef std::function<std::vector<std::string>(void)> GetContentFunc;

        FileSystemAttachmentTextPreview(GetNameFunc getNameFunc, GetContentFunc getContentFunc)
            : m_GetNameFunc(getNameFunc)
            , m_GetContentFunc(getContentFunc)
        {}

    private:
        virtual std::string GetName() const
        {
            return m_GetNameFunc();
        }

        virtual std::string GetDescription() const
        {
            return std::string();
        }

        virtual std::vector<std::string> GetContent() const
        {
            return m_GetContentFunc();
        }

    private:
        GetNameFunc m_GetNameFunc;
        GetContentFunc m_GetContentFunc;
    };
}

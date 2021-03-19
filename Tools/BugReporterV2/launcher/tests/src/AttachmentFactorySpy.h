#pragma once

#include "attachment/AttachmentFactoryDummy.h"

#include <vector>
#include <string>

namespace ureport
{
namespace test
{
    class AttachmentFactorySpy : public AttachmentFactoryDummy
    {
    public:
        mutable std::vector<std::string> m_Calls;

        AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const override
        {
            m_Calls.push_back(std::string("file:") + path);
            return nullptr;
        }

        AttachmentPtr CreateDirectoryAttachment(const std::string& path,
            const std::string& description) const override
        {
            m_Calls.push_back(std::string("directory:") + path);
            return nullptr;
        }
    };
}
}

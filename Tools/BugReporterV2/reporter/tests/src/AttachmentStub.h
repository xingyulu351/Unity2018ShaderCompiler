#pragma once

#include "attachment/AttachmentDummy.h"
#include "FileStub.h"

namespace ureport
{
namespace test
{
    class AttachmentStub : public AttachmentDummy
    {
        std::string m_Path;

    public:
        AttachmentStub(const std::string& path)
            : m_Path(path)
        {
        }

        std::shared_ptr<File> Pack(FilesPacker&, LongTermOperObserver*)
        {
            return std::make_shared<FileStub>(m_Path);
        }
    };
}
}

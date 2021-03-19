#pragma once

#include "attachment/AttachmentDummy.h"
#include "FileStub.h"

namespace ureport
{
namespace test
{
    class FileAttachmentDummy : public AttachmentDummy
    {
    public:
        std::string m_Path;

        FileAttachmentDummy(const std::string& path)
            : m_Path(path)
        {
        }

        std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver*) override
        {
            return std::make_shared<FileStub>(m_Path);
        }
    };
}
}

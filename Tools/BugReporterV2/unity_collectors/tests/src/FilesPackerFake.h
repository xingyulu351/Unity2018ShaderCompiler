#pragma once

#include "reporter/FilesPacker.h"
#include "FileDummy.h"

namespace ureport
{
namespace test
{
    class FilesPackerFake : public ureport::FilesPacker
    {
        std::unique_ptr<File> Pack(const FileSet& files)
        {
            return std::unique_ptr<File>(new FileDummy());
        }
    };
}
}

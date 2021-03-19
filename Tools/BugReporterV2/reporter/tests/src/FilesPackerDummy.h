#pragma once

#include "../src/FilesPacker.h"

namespace ureport
{
namespace test
{
    class FilesPackerDummy : public ureport::FilesPacker
    {
        std::unique_ptr<File> Pack(FileSet& files, LongTermOperObserver*)
        {
            return std::unique_ptr<File>();
        }
    };
}
}

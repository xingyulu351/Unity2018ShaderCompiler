#pragma once

#include <string>
#include <vector>
#include <memory>

namespace ureport
{
    class File;
    class LongTermOperObserver;

    class FilesPacker
    {
    public:
        typedef std::shared_ptr<File> FileRef;

        typedef std::vector<FileRef> FileSet;

        virtual std::unique_ptr<File> Pack(
            FileSet&, LongTermOperObserver*) = 0;

        virtual ~FilesPacker() {}
    };
}

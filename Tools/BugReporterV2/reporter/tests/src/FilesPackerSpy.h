#pragma once

#include "../src/FilesPacker.h"

#include <algorithm>
#include <iterator>

namespace ureport
{
namespace test
{
    class FilesPackerSpy : public ureport::FilesPacker
    {
        FileSet m_Files;

    public:
        FileSet GetFiles() const
        {
            return m_Files;
        }

        std::unique_ptr<File> Pack(FileSet& files, LongTermOperObserver*)
        {
            std::copy(files.begin(), files.end(), std::back_inserter(m_Files));
            return std::unique_ptr<File>();
        }
    };
}
}

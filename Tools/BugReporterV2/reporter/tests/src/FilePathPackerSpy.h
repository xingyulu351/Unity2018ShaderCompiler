#pragma once

#include "../src/FilesPacker.h"
#include "file_system/File.h"

#include <algorithm>
#include <iterator>

namespace ureport
{
namespace test
{
    class FilePathPackerSpy : public ureport::FilesPacker
    {
    public:
        std::vector<std::string> m_Paths;

        std::unique_ptr<File> Pack(FileSet& files, LongTermOperObserver*)
        {
            std::for_each(files.begin(), files.end(), [&](const std::shared_ptr<File>& file) {
                m_Paths.push_back(file->GetPath());
            });
            return std::unique_ptr<File>();
        }
    };
}
}

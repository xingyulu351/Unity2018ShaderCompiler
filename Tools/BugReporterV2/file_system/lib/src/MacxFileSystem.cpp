#include "FileSystem.h"
#include "common/StringAccessShims.h"

#include <memory>
#include <cstdio>

namespace ureport
{
namespace details
{
    class MacxFileSystem : public ureport::FileSystem
    {
        bool HasPath(const std::string& path) const
        {
            auto f = fopen(path.c_str(), "r");
            if (f)
                fclose(f);
            return 0 != f;
        }
    };
}

    std::unique_ptr<FileSystem> CreateFileSystem()
    {
        return std::unique_ptr<FileSystem>(new details::MacxFileSystem());
    }
}

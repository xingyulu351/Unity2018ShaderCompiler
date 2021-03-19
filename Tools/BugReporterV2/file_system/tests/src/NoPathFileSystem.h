#include "FileSystem.h"

namespace ureport
{
namespace test
{
    class NoPathFileSystem : public ureport::FileSystem
    {
        bool HasPath(const std::string& path) const
        {
            return false;
        }

        std::list<std::string> GetDirEntries(const std::string& path,
            const std::string& nameFilter) const
        {
            return std::list<std::string>();
        }

        bool IsDir(const std::string& path) const
        {
            return false;
        }

        size_t GetPathDepth(const std::string& path) const override
        {
            return 0;
        }

        size_t GetSize(const std::string& path) const override
        {
            return 0;
        }
    };
}
}

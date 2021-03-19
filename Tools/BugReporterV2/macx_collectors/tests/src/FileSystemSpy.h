#include "FileSystemDummy.h"

#include <string>
#include <vector>

namespace ureport
{
namespace test
{
    class FileSystemSpy : public FileSystemDummy
    {
    public:
        mutable std::vector<std::string> m_Calls;

        std::list<std::string> GetDirEntries(const std::string& path,
            const std::string& filter) const
        {
            m_Calls.push_back("GetDirEntries path:" + path);
            return FileSystemDummy::GetDirEntries(path, filter);
        }
    };
}
}

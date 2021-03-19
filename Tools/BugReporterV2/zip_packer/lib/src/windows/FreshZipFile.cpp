#include "FreshZipFile.h"

#include <Windows.h>

namespace ureport
{
namespace details
{
    std::shared_ptr<FILE> FreshZipFile::OpenFile(const std::string& path)
    {
        auto const deleter = [] (FILE* file) {
                auto const closed = fclose(file);
                assert(closed == 0);
            };
        const auto wide_path = Widen(path);
        FILE* file = _wfopen(wide_path.c_str(), L"wb");
        return std::shared_ptr<FILE>(file, deleter);
    }
}
}

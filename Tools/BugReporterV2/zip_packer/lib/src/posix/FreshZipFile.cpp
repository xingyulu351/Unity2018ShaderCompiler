#include <cassert>
#include <cstdio>

#include "FreshZipFile.h"

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
        FILE* file = fopen(path.c_str(), "wb");
        return std::shared_ptr<FILE>(file, deleter);
    }
}
}

#include "system_interplay/FileSystem.h"
#include "common/StringUtils.h"

namespace ureport
{
    std::ifstream* OpenFileStream(const std::string& path, std::ios_base::openmode mode)
    {
        return new std::ifstream(Widen(path), mode);
    }
}

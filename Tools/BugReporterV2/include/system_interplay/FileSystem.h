#pragma once

#include <fstream>

namespace ureport
{
    std::ifstream* OpenFileStream(const std::string& path,
        std::ios_base::openmode mode = std::ios_base::in);

    inline void CloseFileStream(std::ifstream *stream) { delete stream; }
}

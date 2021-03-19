#include "TemporaryFile.h"
#include "shims/attribute/GetCString.h"

#include <cstdio>

namespace ureport
{
namespace details
{
    TemporaryFile::~TemporaryFile()
    {
        const auto path = m_File->GetPath();
        m_File.reset();
        std::remove(GetCString(path));
    }
}
}

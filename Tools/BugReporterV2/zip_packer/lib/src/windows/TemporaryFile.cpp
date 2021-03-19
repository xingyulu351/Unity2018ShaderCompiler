#include "TemporaryFile.h"
#include "common/StringUtils.h"
#include "shims/attribute/GetCString.h"

#include <stdio.h>

namespace ureport
{
namespace details
{
    TemporaryFile::~TemporaryFile()
    {
        const auto path = m_File->GetPath();
        m_File.reset();
        ::_wremove(GetCString(Widen(path)));
    }
}
}

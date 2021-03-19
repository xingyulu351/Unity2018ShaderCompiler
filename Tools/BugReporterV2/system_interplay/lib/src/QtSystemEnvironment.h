#pragma once

#include "system_interplay/SystemEnvironment.h"

#include <qdir.h>

namespace ureport
{
namespace details
{
    class QtSystemEnvironment : public ureport::SystemEnvironment
    {
        std::string GetTempDirectory() const override
        {
            return QDir::tempPath().toStdString();
        }
    };
}
}

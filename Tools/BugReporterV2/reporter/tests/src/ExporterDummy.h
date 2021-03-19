#pragma once

#include "reporter/Exporter.h"

namespace ureport
{
namespace test
{
    class ExporterDummy : public Exporter
    {
        void Save(const UnityBugMessage& message, LongTermOperObserver*, const std::string& path)
        {
        }
    };
}
}

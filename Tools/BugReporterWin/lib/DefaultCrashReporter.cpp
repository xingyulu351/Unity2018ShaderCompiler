#include "DefaultCrashReporter.h"

#include <Windows.h>

namespace unity
{
    void DefaultCrashReporter::SetValue(const std::string& name, const std::string& value)
    {
    }

    void DefaultCrashReporter::ReportFile(const std::string& name, const std::string& path)
    {
    }

    bool DefaultCrashReporter::Report(const std::string& options)
    {
        return true;
    }
}

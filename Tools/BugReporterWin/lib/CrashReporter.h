#pragma once

#include <string>

namespace unity
{
    class CrashReporter
    {
    public:
        virtual void SetValue(const std::string& name, const std::string& value) = 0;

        virtual void ReportFile(const std::string& name, const std::string& path) = 0;

        virtual bool Report(const std::string& options) = 0;

        virtual ~CrashReporter() {}
    };
}

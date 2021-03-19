#include "CrashReporter.h"

namespace unity
{
    class DefaultCrashReporter : public CrashReporter
    {
        void SetValue(const std::string& name, const std::string& value);

        void ReportFile(const std::string& name, const std::string& path);

        bool Report(const std::string& options);
    };
}

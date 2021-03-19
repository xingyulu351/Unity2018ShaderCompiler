#pragma once

#include <memory>
#include <string>
#include <list>
#include <vector>

namespace ureport
{
    class FileSystem;

namespace macosx
{
    class CrashReport;

    class CrashReportsProvider
    {
        std::shared_ptr<FileSystem> m_FileSystem;

    public:
        static const std::string kDiagnosticReportsPath;
        static const std::string kCrashReporterPath;

    public:
        CrashReportsProvider(std::shared_ptr<FileSystem> fileSystem);

        virtual ~CrashReportsProvider();

        virtual std::vector<CrashReport> GetAllReports() const;

    private:
        std::list<std::string> GetAllReportFiles() const;

        bool IsCrashReportPath(const std::string& path) const;
    };
}
}

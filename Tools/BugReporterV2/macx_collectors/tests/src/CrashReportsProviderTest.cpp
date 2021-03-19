#include "FileSystemSpy.h"
#include "CrashReportsProvider.h"
#include "CrashReport.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>

SUITE(CrashReportsProvider)
{
    using namespace ::ureport;
    using namespace ::ureport::macosx;
    using namespace ::ureport::test;
    using namespace ::UnitTest;

    class AllPathsExist : public FileSystemSpy
    {
        bool HasPath(const std::string& path) const
        {
            return true;
        }
    };

    class NoPathsExist : public FileSystemSpy
    {
        bool HasPath(const std::string& path) const
        {
            return false;
        }
    };

    class NoCrashReportFileSystem : public AllPathsExist
    {
        std::list<std::string> GetDirEntries(const std::string& path,
            const std::string& filter) const
        {
            return std::list<std::string>();
        }
    };

    class ReportsFileSystemStub : public AllPathsExist
    {
    public:
        std::list<std::string> m_Reports;

        std::list<std::string> GetDirEntries(const std::string& path,
            const std::string& filter) const
        {
            return m_Reports;
        }
    };

    TEST(GetAllReports_GetsReportsFromDiagnosticReportsDirectory)
    {
        auto const fileSystem = std::make_shared<AllPathsExist>();
        CrashReportsProvider provider(fileSystem);
        provider.GetAllReports();
        CHECK_EQUAL("GetDirEntries path:" + CrashReportsProvider::kDiagnosticReportsPath,
            fileSystem->m_Calls.at(0));
    }

    TEST(GetAllReports_GetsReportsFromCrashReporterDirectoryWhenDiagnosticReportsDoesNotExists)
    {
        auto const fileSystem = std::make_shared<NoPathsExist>();
        CrashReportsProvider provider(fileSystem);
        provider.GetAllReports();
        CHECK_EQUAL("GetDirEntries path:" + CrashReportsProvider::kCrashReporterPath,
            fileSystem->m_Calls.at(0));
    }

    TEST(GetAllReports_ReturnsEmptySequenceWhenNoReport)
    {
        CrashReportsProvider provider(std::make_shared<NoCrashReportFileSystem>());
        auto const reports = provider.GetAllReports();
        CHECK(IsEmpty(reports));
    }

    TEST(GetAllReports_ReturnsReportsExistInFileSystem)
    {
        auto fileSystem = std::make_shared<ReportsFileSystemStub>();
        fileSystem->m_Reports.push_back("first.crash");
        fileSystem->m_Reports.push_back("second.crash");
        CrashReportsProvider provider(fileSystem);
        auto const reports = provider.GetAllReports();
        CHECK_EQUAL(2, GetSize(reports));
        CHECK_EQUAL("first.crash", reports.at(0).GetPath());
        CHECK_EQUAL("second.crash", reports.at(1).GetPath());
    }

    TEST(GetAllReports_SkipsNotCrashReportPath)
    {
        auto fileSystem = std::make_shared<ReportsFileSystemStub>();
        fileSystem->m_Reports.push_back("not a report");
        CrashReportsProvider provider(fileSystem);
        auto const reports = provider.GetAllReports();
        CHECK_EQUAL(0, GetSize(reports));
    }

    TEST(GetAllReports_ReturnsReportsForPathsThatEndsWithDotCrash)
    {
        auto fileSystem = std::make_shared<ReportsFileSystemStub>();
        fileSystem->m_Reports.push_back("not a report");
        fileSystem->m_Reports.push_back("first.crash");
        fileSystem->m_Reports.push_back("not .crash report");
        fileSystem->m_Reports.push_back("second.crash");
        fileSystem->m_Reports.push_back(".crash report");
        CrashReportsProvider provider(fileSystem);
        auto const reports = provider.GetAllReports();
        CHECK_EQUAL(2, GetSize(reports));
        CHECK_EQUAL("first.crash", reports.at(0).GetPath());
        CHECK_EQUAL("second.crash", reports.at(1).GetPath());
    }
}

#include "CrashReport.h"

#include <UnitTest++.h>

SUITE(CrashReport)
{
    using namespace ::ureport;
    using namespace ::ureport::macosx;

    TEST(MakeFromPathReturnsReportContainsThePath)
    {
        auto const report = CrashReport::MakeFromPath("path");
        CHECK_EQUAL("path", report.GetPath());
    }

    TEST(MakeFromPath_WhenPathIsEmpty_ThrowsEmptyReportPathException)
    {
        CHECK_THROW(CrashReport::MakeFromPath(""), CrashReport::EmptyReportPathException);
    }

    TEST(MakeFromPath_UsesPathBaseNameAsReportApplication)
    {
        CHECK_EQUAL("name", CrashReport::MakeFromPath("name").GetApplication());
        CHECK_EQUAL("name", CrashReport::MakeFromPath("name.suffix").GetApplication());
        CHECK_EQUAL("name", CrashReport::MakeFromPath("/name.suffix").GetApplication());
        CHECK_EQUAL("name", CrashReport::MakeFromPath("/foo/name.suffix").GetApplication());
        CHECK_EQUAL("name", CrashReport::MakeFromPath("./name.suffix").GetApplication());
    }

    TEST(MakeFromPath_UsesPartOfBaseNameBetweenUnderscoresAsTimestamp)
    {
        CHECK_EQUAL("time-stamp",
            CrashReport::MakeFromPath("./dir/name_time-stamp.crash").GetTimestamp());
        CHECK_EQUAL("time-stamp",
            CrashReport::MakeFromPath("./dir/name_time-stamp_name.crash").GetTimestamp());
        CHECK_EQUAL("time-stamp",
            CrashReport::MakeFromPath("name_time-stamp_name").GetTimestamp());
    }
}

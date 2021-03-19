#pragma once

#include "../src/CrashReportsProvider.h"

namespace ureport
{
namespace test
{
    class CrashReportsProviderDummy : public ::ureport::macosx::CrashReportsProvider
    {
    public:
        CrashReportsProviderDummy()
            : CrashReportsProvider(nullptr)
        {
        }

        std::vector< ::ureport::macosx::CrashReport> GetAllReports() const
        {
            return std::vector< ::ureport::macosx::CrashReport>();
        }
    };
}
}

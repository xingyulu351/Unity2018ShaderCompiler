#include "OSInfoCollector.h"
#include "ReportFactSpy.h"
#include "SystemInfoProviderFake.h"
#include "test/AssertContains.h"

#include <UnitTest++.h>
#include <tuple>

SUITE(OSInfoCollector)
{
    using namespace ureport;
    using namespace ureport::test;
    using namespace ureport::system_collectors;

    TEST(Collect_WritesOSNameFact)
    {
        OSInfoCollector collector(std::make_shared<OSNameSystemInfoProviderFake>("OS name"));
        ReportFactSpy report;
        collector.Collect(report);
        AssertContains(report.m_Facts, std::make_tuple("OSName", "OS name"));
    }

    TEST(Collect_WritesOSVersionFact)
    {
        const Version version(1, 2, 3);
        OSInfoCollector collector(std::make_shared<OSVersionSystemInfoProviderFake>(version));
        ReportFactSpy report;
        collector.Collect(report);
        const auto expectedFact = std::make_tuple("OSVersion", version.GetTextualForm());
        AssertContains(report.m_Facts, expectedFact);
    }
}

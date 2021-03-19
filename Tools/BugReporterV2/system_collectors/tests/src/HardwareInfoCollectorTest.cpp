#include "HardwareInfoCollector.h"
#include "HardwareInfoProviderFake.h"
#include "ReportFactSpy.h"
#include "test/AssertContains.h"

#include <UnitTest++.h>
#include <tuple>
#include <memory>

SUITE(HardwareInfoCollector)
{
    using namespace ureport;
    using namespace ureport::test;
    using namespace ureport::system_collectors;

    TEST(Collect_WritesBasicHardwareTypeFact)
    {
        HardwareInfoCollector collector(
            std::make_shared<BasicHardwareTypeProviderFake>("Basic hardware type"));
        ReportFactSpy report;
        collector.Collect(report);
        AssertContains(report.m_Facts, std::make_tuple("BasicHardwareType", "Basic hardware type"));
    }

    TEST(Collect_WritesGraphicsHardwareTypeFact)
    {
        HardwareInfoCollector collector(
            std::make_shared<GraphicsHardwareTypeProviderFake>("Graphics type"));
        ReportFactSpy report;
        collector.Collect(report);
        AssertContains(report.m_Facts, std::make_tuple("GraphicsHardwareType", "Graphics type"));
    }
}

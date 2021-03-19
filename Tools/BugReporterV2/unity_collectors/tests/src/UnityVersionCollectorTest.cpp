#include "UnityVersionCollector.h"
#include "reporter/ReportDraft.h"
#include "UnityVersionFake.h"
#include <UnitTest++.h>

using namespace ureport;
using namespace ureport::unity_collectors;

SUITE(UnityVersionCollector)
{
    TEST(Collect_PutsUnityVersionIntoReport)
    {
        std::shared_ptr<UnityVersion> unityVersion(new test::UnityVersionFake());
        std::unique_ptr<Collector> collector(new UnityVersionCollector(unityVersion));
        auto report = CreateNewReport();
        collector->Collect(*report);
        CHECK(report->ReadFact("UnityVersion") != "");
    }
}

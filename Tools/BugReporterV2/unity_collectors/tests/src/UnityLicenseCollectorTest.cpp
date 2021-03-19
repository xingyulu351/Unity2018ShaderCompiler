#include "UnityLicenseCollector.h"
#include "reporter/ReportDraft.h"

#include <UnitTest++.h>

using namespace ureport;
using namespace ureport::unity_collectors;

SUITE(UnityLicenseCollector)
{
    class TestableCollector : public UnityLicenseCollector
    {
    protected:
        TestableCollector()
            : m_Report(CreateNewReport())
        {
        }

        std::unique_ptr<std::istream> GetLicense() const
        {
            return std::unique_ptr<std::istream>(new std::stringstream(m_License));
        }

    protected:
        std::shared_ptr<ReportDraft> m_Report;
        std::string m_License;
    };

    TEST_FIXTURE(TestableCollector, Collect_GivenLicenseHasOnlyFeatureValue0_WritesProUserLicenseTypeFact)
    {
        m_License = "<Feature Value=\"0\"/>";
        Collect(*m_Report);
        CHECK_EQUAL("Pro" , m_Report->ReadFact("LicenseType"));
    }

    TEST_FIXTURE(TestableCollector, Collect_GivenEmptyLicense_WritesNoLicenseTypeFact)
    {
        m_License = "";
        Collect(*m_Report);
        CHECK_EQUAL("" , m_Report->ReadFact("LicenseType"));
    }

    TEST_FIXTURE(TestableCollector, Collect_GivenLicenseWithNoFeature_WritesNoLicenseTypeFact)
    {
        m_License = "No feature license";
        Collect(*m_Report);
        CHECK_EQUAL("" , m_Report->ReadFact("LicenseType"));
    }

    TEST_FIXTURE(TestableCollector, Collect_GivenLicenseWithManyFeatureValuesContainsNoZeroFeatureValue_WritesNoLicenseTypeFact)
    {
        m_License =
            "<Feature Value=\"1\"/>\n"
            "<Feature Value=\"2\"/>\n"
            "<Feature Value=\"3\"/>\n";
        Collect(*m_Report);
        CHECK_EQUAL("" , m_Report->ReadFact("LicenseType"));
    }

    TEST_FIXTURE(TestableCollector, Collect_GivenLicenseWithManyFeatureValuesContainsZeroFeatureValue_WritesProUserLicenseTypeFact)
    {
        m_License =
            "<Feature Value=\"2\"/>\n"
            "<Feature Value=\"1\"/>\n"
            "<Feature Value=\"0\"/>\n";
        Collect(*m_Report);
        CHECK_EQUAL("Pro" , m_Report->ReadFact("LicenseType"));
    }

    TEST_FIXTURE(TestableCollector, Collect_LicenseWithFreeFeature_WritesFreeLicenseType)
    {
        m_License = "<Feature Value=\"62\"/>";
        Collect(*m_Report);
        CHECK_EQUAL("Free" , m_Report->ReadFact("LicenseType"));
    }
}
